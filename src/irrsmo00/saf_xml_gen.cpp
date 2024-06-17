#include "saf_xml_gen.hpp"

#include <regex>
#include <string>
#include <iostream>
#include <unistd.h>

//Public Functions of XmlGen
char * XmlGen::build_xml_string(
    char * json_req_string,
    char * userid_buffer,
    unsigned char * opcode,
    int * irrsmo00_options,
    unsigned int * result_buffer_size,
    bool * debug
) {
    //Main body function that builds an xml string
    nlohmann::json request;
    request = nlohmann::json::parse(json_req_string);
    std::string requestOperation, adminType, profileName, operation, runningUserId = "";
    //Build the securityrequest tag (Consistent)
    build_open_tag("securityrequest");
    build_attribute(make_xml_attribute("xmlns","http://www.ibm.com/systems/zos/saf"));
    build_attribute(make_xml_attribute("xmlns:racf","http://www.ibm.com/systems/zos/racf"));
    build_end_nested_tag();

    //Obtain JSON Header information and Build into Admin Object where appropriate
    for (const auto& item : request.items()) { 
        // requestData contains no Header information and is ignored
        if ( item.key().compare("requestData") == 0 ) { continue; }
        // The following options dictate parameters to IRRSMO00 and are not built into XML 
        else if ( item.key().compare("runningUserId") == 0 ) { runningUserId = item.value().get<std::string>(); }
        else if ( item.key().compare("resultBufferSize") == 0 ) { *result_buffer_size = item.value().get<uint>(); }
        else if ( item.key().compare("debugmode") == 0 ) { *debug = item.value().get<bool>(); }
        // All other valid header information should be built into the XML
        else if ( item.key().compare("adminType") == 0 ) {
            // The type of administrative object we are working with
            adminType = item.value().get<std::string>();
            build_open_tag(adminType);
        }
        else if ( item.key().compare("profileName") == 0 ) {
            // The name of the target profile
            profileName = item.value().get<std::string>();
            build_attribute(make_xml_attribute("name",profileName));
        }
        else if ( item.key().compare("requestOperation") == 0 ) {
            // The type of request we are performing
            requestOperation = item.value().get<std::string>();
            operation = convert_operation(requestOperation,opcode,irrsmo00_options);
            build_attribute("operation=\""+operation+"\"");
        }
        else if ( item.value().is_string() ) {
            //All other attribute information is built into the xml at this level to account for VOLUME/GENERIC/Others
            build_attribute(make_xml_attribute(item.key(),item.value().get<std::string>())); 
        }
    }

    if (!runningUserId.empty()) {
        //Run this command as another user id
        const int userid_length = runningUserId.length();
        strncpy(userid_buffer, runningUserId.c_str(), userid_length);
        convert_to_ebcdic(userid_buffer, userid_length);
    }

    build_attribute(make_xml_attribute("requestid",adminType+"_request"));

    if ((request[requestOperation].contains("requestData")) && (!request[requestOperation].empty()))
    {
        build_end_nested_tag();

        //Build the request data (segment-trait information)
        for (const auto& item : request[requestOperation]["requestData"].items())
        {
            build_open_tag(item.key());
            build_end_nested_tag();
            //Build each individual trait
            for (const auto& trait : request[requestOperation]["requestData"][item.key()].items())
            {
                std::string operation = (trait.value()["operation"].is_null()) ? "set" : trait.value()["operation"].get<std::string>();
                std::string value = (trait.value()["value"].is_boolean()) ? "" : trait.value()["value"].get<std::string>();
                build_single_trait(trait.key(), operation, value);
            }
            build_full_close_tag(item.key());
        }
        
        //Close the admin object
        build_full_close_tag(requestOperation);

        //Close the securityrequest tag (Consistent)
        build_full_close_tag("securityrequest");
    }
    else
    {
        //Close the admin object
        build_close_tag_no_value();
    }
    
    if (*debug)
    {
        //print information in debug mode
        std::cout << "XML Request string (Ascii): " << xml_buffer << "\n";
    }

    //convert our c++ string to a char * buffer
    const int length = xml_buffer.length();
    char* output_buffer = new char[length + 1];
    strncpy(output_buffer, xml_buffer.c_str(), length+1);
    convert_to_ebcdic(output_buffer, length);

    if (*debug)
    {
        //print information in debug mode
        std::cout << std::hex << "XML Request string (Ebcdic): " << std::hex << cast_hex_string(output_buffer) << "\n";
    }

    return output_buffer;
}

//Private Functions of XmlGen
void XmlGen::build_open_tag(std::string tag) {
    //Ex: "<base:universal_access"
    xml_buffer.append("<"+tag);
}
void XmlGen::build_attribute(std::string attribute) {
    //Ex: " operation=set"
    xml_buffer.append(" "+attribute);
}
void XmlGen::build_value(std::string value) {
    //Ex: ">Read"
    xml_buffer.append(">"+value);
}
void XmlGen::build_end_nested_tag() {
    xml_buffer.append(">");
}
void XmlGen::build_full_close_tag(std::string tag) {
    //Ex: "</base:universal_access>"
    xml_buffer.append("</"+tag+">");
}
void XmlGen::build_close_tag_no_value() {
    xml_buffer.append("/>");
}
void XmlGen::build_single_trait(
    std::string tag,
    std::string operation,
    std::string value
) {
    //Combines above functions to build "trait" tags with added options and values
    //Ex: "<base:universal_access operation=set>Read</base:universal_access>"
    build_open_tag(tag);
    if (operation.length() != 0) { build_attribute(make_xml_attribute("operation",operation)); }
    if (value.length() == 0) { build_close_tag_no_value(); }
    else {
        build_value(value);
        build_full_close_tag(tag);
    }
}
std::string XmlGen::make_xml_attribute(
    std::string name,
    std::string value
) {
    //Builds "attribute" string used by build_attribute
    //Ex: make_xml_attribute(operation,set) = "operation=set"
    std::string output = name;
    output.append("=\""+value+"\"");
    return output;
}

std::string XmlGen::convert_operation(
    std::string requestOperation,
    unsigned char * opcode,
    int * irrsmo00_options
) {
    //Converts the designated function to a short OPCODE, the correct IRRSMO00 operation
    //and adjusts IRRSMO00 options as necessary (alter operations require the PRECHECK attribute)
    if (requestOperation.compare("add") == 0)  {
        *opcode = OP_ADD;
        return "set";
    }
    if (requestOperation.compare("alter") == 0) {
        *opcode = OP_ALT;
        *irrsmo00_options = 15;
        return "set";
    }
    if (requestOperation.compare("delete") == 0) {
        *opcode = OP_DEL;
        return "delete";
    }
    if (requestOperation.compare("extract") == 0) {
        *opcode = OP_LST;
        return "listdata";
    }
    return "";
}

void XmlGen::convert_to_ebcdic(
    char * ascii_str,
    int length
) {
    //Universal function to convert ascii string to EBCDIC-1047 in-place
    #ifndef __MVS__
    for(int i = 0; i < length; i++)
    {
        *(ascii_str+i) = AsciiToEbcdic[(unsigned char)*(ascii_str+i)];
    }
    #else
    __a2e_s(ascii_str);
    #endif //__MVS__
}

// Connects the "XML library" to the C layer with these extern C functions

extern char * injson_to_inxml(
    char * injson,
    char * userid_buffer,
    unsigned char * opcode,
    int * irrsmo00_options,
    unsigned int * result_buffer_size,
    bool * debug
) {
    //Build an XMLGen XML Generator object and build an IRRSMO00
    //request xml string from a supplied JSON string
    XmlGen * xml = new XmlGen();
    return xml->build_xml_string(injson, userid_buffer, opcode, irrsmo00_options, result_buffer_size, debug);
}
