#include "saf_xml.hpp"

#include <regex>
#include <string>
#include <iostream>
#include <unistd.h>

//Private functions
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
void XmlGen::build_single_trait(std::string tag, std::string operation, std::string value) {
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
std::string XmlGen::make_xml_attribute(std::string name, std::string value)
{
    //Builds "attribute" string used by build_attribute
    //Ex: make_xml_attribute(operation,set) = "operation=set"
    std::string output = name;
    output.append("=\""+value+"\"");
    return output;
}

std::string XmlGen::convert_operation(std::string requestOperation, unsigned char * opcode, int * irrsmo00_options)
{
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

void XmlGen::convert_to_ebcdic(char * ascii_str, int length){
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

char * XmlGen::build_xml_string(
    char * json_req_string, char * userid_buffer, unsigned char * opcode,
    int * irrsmo00_options, unsigned int * result_buffer_size, bool * debug
){
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

void XmlParse::parse_header_attributes(nlohmann::json * input_json, std::string header_string)
{
    //Parse the header attributes of the XML for JSON information
    std::smatch attr_sub_re_match;
    std::regex attr {R"~(([a-z]*)="([^ ]*)")~"};

    std::string::size_type n = 0, n_old;
    std::string sel_str;
    do
    {
        n_old = (n == 0) ? n:n+1;
        n = header_string.substr(n_old,header_string.length()-n_old).find(' ') + n_old;
        sel_str = header_string.substr(n_old,n - n_old);
        if (regex_match(sel_str, attr_sub_re_match, attr))
        {
            //std::cout << attr_sub_re_match[0] << '\n'; // Full std::string
            //std::cout << attr_sub_re_match[1] << '\n'; // Attribute Key
            //std::cout << attr_sub_re_match[2] << '\n'; // Attribute Value
            (*input_json)[attr_sub_re_match[1]] = attr_sub_re_match[2];
        }
    }while(n_old != n + 1);
};

void XmlParse::parse_outer_xml(nlohmann::json * input_json, std::string body_string)
{
    //Parse the outer layer of the body XML for tags within, then use another function to parse those "inner" tags
    std::regex outer_tag {R"(<([a-z]*)>.*</([a-z]*)>)"};
    std::smatch body_sub_re_match, body_iter_sub_re_match;
    
    std::string::size_type n = 0, n_old;
    std::string sel_str, current_tag, current_end_tag, remaining_str;

    if(regex_match(body_string, body_sub_re_match, outer_tag))
    {
        current_tag = body_sub_re_match[1];
        std::regex inner_tag {"<"+current_tag+R"(>(.*?)</)"+current_tag+">"};
        do
        {
            n_old = (n == 0) ? n:n+current_end_tag.length();
            current_end_tag = R"(</)"+current_tag+">";
            std::regex inner_tag {"<"+current_tag+R"(>(.*?)</)"+current_tag+">"};
            n = body_string.substr(n_old,body_string.length()-n_old).find(current_end_tag) + n_old;
            sel_str = body_string.substr(n_old,n + current_end_tag.length() - n_old);
            //std::cout << "Selection std::string: " << sel_str << "\n";
            if (regex_match(sel_str, body_iter_sub_re_match, inner_tag))
            {
                //std::cout << "Full std::string: " << body_iter_sub_re_match[0] << '\n'; // Full std::string
                //std::cout << "Inner Tags: " << body_iter_sub_re_match[1] << '\n'; // Inner Tags
                parse_inner_xml(input_json, body_iter_sub_re_match[1], current_tag);

            }

            remaining_str = body_string.substr(n+current_end_tag.length(),body_string.length()-current_end_tag.length()-n);
            //std::cout << "Remaining std::string: " << remaining_str << "\n";
            if (regex_match(remaining_str, body_iter_sub_re_match, outer_tag))
            {
                current_tag = body_iter_sub_re_match[1];
                //std::cout << "Updating Current Tag: " << current_tag << "\n";
            }
        }while(n + current_end_tag.length() < body_string.length());
    }
};

void XmlParse::parse_inner_xml(nlohmann::json * input_json, std::string inner_data, std::string outer_tag)
{
    if (inner_data.find("<") == std::string::npos)
    {
        //std::cout << "Updating " << outer_tag << " with value: " << inner_data << "\n";
        update_json(input_json, inner_data, outer_tag);
        return;
    }
    nlohmann::json _string;
    parse_outer_xml(&_string, inner_data);
    update_json(input_json, _string, outer_tag);
}

void XmlParse::update_json(nlohmann::json * input_json, nlohmann::json inner_data, std::string outer_tag)
{
    if (!((*input_json).contains(outer_tag) || (*input_json).contains(outer_tag+"s")))
    {
        (*input_json)[outer_tag] = inner_data;
        return;
    }
    if ((*input_json).contains(outer_tag))
    {
        (*input_json)[outer_tag+"s"] = {(*input_json)[outer_tag], inner_data};
        (*input_json).erase(outer_tag);
    }
    else
    {
        (*input_json)[outer_tag+"s"].push_back(inner_data);
    }
}

void XmlParse::convert_to_ascii(char * ebcdic_str, int length)
{
    //Universal function to convert EBCDIC-1047 string to ascii in place
    #ifndef __MVS__
    for(int i = 0; i < length; i++)
    {
        *(ebcdic_str+i) = EbcdicToAscii[(unsigned char)*(ebcdic_str+i)];
    }
    #else
    __e2a_s(ebcdic_str);
    #endif //__MVS__
}

char * XmlParse::build_json_string(char * xml_result_string, unsigned char opcode, int saf_rc, int racf_rc, int racf_rsn, bool debug)
{

    //TODO: Pass raw data back as well
    
    if (debug)
    {
        //print information in debug mode
        std::cout << "XML Result string (Ebcdic): " << std::hex << cast_hex_string( xml_result_string ) << "\n";
    }


    convert_to_ascii(xml_result_string, strlen(xml_result_string));
    xml_buffer = xml_result_string;

    if (debug)
    {
        //print information in debug mode
        std::cout << "XML Result string (Ascii): " << xml_buffer << "\n";
    }

    std::smatch xml_sub_re_match;
    std::regex full_xml {R"~(<\?xml version="1\.0" encoding="IBM-1047"\?><securityresult xmlns="http:\/\/www\.ibm\.com\/systems\/zos\/saf\/IRRSMO00Result1"><([a-z]*) ([^>]*)>(<.+>)<\/securityresult>)~"}; 
    //Regular expression designed to match the header, generic body, and closing tags of the xml

    nlohmann::json result_json;
    nlohmann::json result;
    nlohmann::json returnCodes;

    std::string profile_type, profile_close_tag, profile_xml_attrs, profile_xml_body;

    if(regex_match(xml_buffer, xml_sub_re_match, full_xml))
    {
    //std::cout << xml_sub_re_match[0] << "\n\n"; // Full std::string
    //std::cout << xml_sub_re_match[1] << '\n'; // Profile Type
    //std::cout << xml_sub_re_match[2] << '\n'; // Profile Attrs
    //std::cout << xml_sub_re_match[3] << '\n'; // Command Information

    //Use sub-matches in the regular expression to pull out useful information from the header of the xml
    profile_type = xml_sub_re_match[1];
    profile_close_tag = R"(</)"+profile_type+">";
    profile_xml_attrs = xml_sub_re_match[2];
    profile_xml_body = xml_sub_re_match[3];

    //Parse out these attributes
    parse_header_attributes(&result, profile_xml_attrs);
    //Erase the profile close tag as it messes up later regex parsing
    profile_xml_body.erase(profile_xml_body.find(profile_close_tag),profile_close_tag.length());
    //Parse the body of the xml here
    parse_outer_xml(&result, profile_xml_body);

    result_json["adminType"] = profile_type;
    switch (opcode) {
        case OP_ADD:
            result_json["requestOperation"] = "add";
            break;
        case OP_ALT:
            result_json["requestOperation"] = "alter";
            break;
        case OP_DEL:
            result_json["requestOperation"] = "delete";
            break;
        case OP_LST:
            result_json["requestOperation"] = "extract";
            break;
        default:
            result_json["requestOperation"] = "unsupported";
    }
    
    //Put the built JSON object in the result JSON
    result_json["result"] = result;
    }
    else
    {
    //If the XML does not match the main regular expression, then return this string to indicate an error
    result_json["error"] = "XML PARSE ERROR: Could not match data to valid xml patterns!";
    }

    //Build a return codes object in the JSON to return IRRSMO00 return and reason codes
    returnCodes["safReturnCode"] = saf_rc;
    returnCodes["racfReturnCode"] = racf_rc;
    returnCodes["racfReasonCode"] = racf_rsn;

    result_json["returnCodes"] = returnCodes;
    
    //Convert c++ string into char * c string
    std::string result_json_as_string = result_json.dump();
    const int length = result_json_as_string.length();
    char* output_buffer = new char[length + 1];
    strncpy(output_buffer, result_json_as_string.c_str(), length+1);
    return output_buffer;
}

std::string cast_hex_string(char * input)
{
    //Cast data to hex so that small strings of hex values can be printed to represent EBCDIC data
    std::string output = "{ ";
    char buff[5];
    for(int i = 0; i < strlen(input); i++)
    {
        if (i > 0) { output += ", "; }
        std::snprintf(buff, 5, "0x%02x", (unsigned char)*(input+i));
        output += buff;
    }
    output += " }";
    return output;
}

// Connects the "XML library" to the C layer with these extern C functions

extern char * injson_to_inxml(char * injson, char * userid_buffer, unsigned char * opcode, int * irrsmo00_options, unsigned int * result_buffer_size, bool * debug){
    XmlGen * xml = new XmlGen();
    return xml->build_xml_string(injson, userid_buffer, opcode, irrsmo00_options, result_buffer_size, debug);
}

extern char * outxml_to_outjson(char * outxml, unsigned char opcode, int saf_rc, int racf_rc, int racf_rsn, bool debug){
    XmlParse * xml = new XmlParse();
    return xml->build_json_string(outxml, opcode, saf_rc, racf_rc, racf_rsn, debug);
}