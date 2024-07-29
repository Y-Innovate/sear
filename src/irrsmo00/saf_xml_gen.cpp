#include "saf_xml_gen.hpp"

#include "key_map.hpp"

#include <regex>
#include <string>
#include <iostream>
#include <unistd.h>

//Public Functions of XmlGen
char * XmlGen::build_xml_string(
    nlohmann::json request,
    char * userid_buffer,
    int * irrsmo00_options,
    unsigned int * result_buffer_size,
    bool * debug
) {
    //Main body function that builds an xml string
    std::string requestOperation, adminType, profileName, className, operation, currentSegment, runningUserId = "";
    nlohmann::json requestData;

    //Build the securityrequest tag (Consistent)
    build_open_tag("securityrequest");
    build_attribute("xmlns","http://www.ibm.com/systems/zos/saf");
    build_attribute("xmlns:racf","http://www.ibm.com/systems/zos/racf");
    build_end_nested_tag();

    //Obtain JSON Header information and Build into Admin Object where appropriate
    for (const auto& item : request.items()) { 
        // requestData contains no Header information and is ignored
        if ( item.key().compare("request_data") == 0 ) { continue; }
        // The following options dictate parameters to IRRSMO00 and are not built into XML 
        else if ( item.key().compare("running_user_id") == 0 ) { runningUserId = item.value().get<std::string>(); }
        else if ( item.key().compare("result_buffer_size") == 0 ) { *result_buffer_size = item.value().get<uint>(); }
        else if ( item.key().compare("debug_mode") == 0 ) { *debug = item.value().get<bool>(); }
        // All other valid header information should be built into the XML
        else if ( item.key().compare("admin_type") == 0 ) {
            // The type of administrative object we are working with
            adminType = item.value().get<std::string>();
            build_open_tag(adminType);
        }
        else if ( item.key().compare("profile_name") == 0 ) {
            // The name of the target profile
            profileName = item.value().get<std::string>();
            build_attribute("name",profileName);
        }
        else if ( item.key().compare("operation") == 0 ) {
            // The type of request we are performing
            requestOperation = item.value().get<std::string>();
            operation = convert_operation(requestOperation,irrsmo00_options);
            build_attribute("operation", operation);
        }
        else if ( item.key().compare("class_name") == 0 ) {
            // The name of the target profile
            className = item.value().get<std::string>();
            build_attribute("class", className);
        }
        else if ( item.value().is_string() ) {
            //All other attribute information is built into the xml at this level to account for VOLUME/GENERIC/Others
            build_attribute(item.key(),item.value().get<std::string>()); 
        }
    }

    if (!runningUserId.empty()) {
        //Run this command as another user id
        const int userid_length = runningUserId.length();
        strncpy(userid_buffer, runningUserId.c_str(), userid_length);
        convert_to_ebcdic(userid_buffer, userid_length);
    }

    build_attribute("requestid",adminType+"_request");

    if ((request.contains("request_data")) && (!request["request_data"].empty()))
    {
        build_end_nested_tag();

        build_request_data(adminType, request["request_data"]);
        
        //Close the admin object
        build_full_close_tag(adminType);

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
    char* output_buffer = (char *)malloc(sizeof(char) * (length + 1));
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
std::string XmlGen::replace_xml_chars(std::string data)
{
    //Replace xml-substituted characters with their substitution strings
    std::string amp = "&amp;", gt = "&gt;", lt = "&lt;", quot = "&quot;", apos = "&apos;";
    std::size_t index;
    for (int i=0; i<data.length(); i++)
    {
        if (data[i] == '&')
        {
            data.replace(i,1,amp,0,amp.length());
            i += amp.length() - 1 - 1;
        }
        if (data[i] == '<')
        {
            data.replace(i,1,lt,0,lt.length());
            i += lt.length() - 1 - 1;
        }
        if (data[i] == '>')
        {
            data.replace(i,1,gt,0,gt.length());
            i += gt.length() - 1 - 1;
        }
        if (data[i] == '"')
        {
            data.replace(i,1,quot,0,quot.length());
            i += quot.length() - 1 - 1;
        }
        if (data[i] == '\'')
        {
            data.replace(i,1,apos,0,apos.length());
            i += apos.length() - 1 - 1;
        }
    }
    return data;
}
void XmlGen::build_open_tag(std::string tag) {
    //Ex: "<base:universal_access"
    tag = replace_xml_chars(tag);
    xml_buffer.append("<"+tag);
}
void XmlGen::build_attribute(
    std::string name,
    std::string value
) {
    //Ex: " operation=set"
    name = replace_xml_chars(name);
    value = replace_xml_chars(value);
    xml_buffer.append(" "+name+"=\""+value+"\"");
}
void XmlGen::build_value(std::string value) {
    //Ex: ">Read"
    value = replace_xml_chars(value);
    xml_buffer.append(">"+value);
}
void XmlGen::build_end_nested_tag() {
    xml_buffer.append(">");
}
void XmlGen::build_full_close_tag(std::string tag) {
    //Ex: "</base:universal_access>"
    tag = replace_xml_chars(tag);
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
    if (operation.length() != 0) { build_attribute("operation",operation); }
    if (value.length() == 0) { build_close_tag_no_value(); }
    else {
        build_value(value);
        build_full_close_tag(tag);
    }
}

void XmlGen::build_request_data(std::string adminType, nlohmann::json requestData) {
    //Builds the xml for request data (segment-trait information) passed in a json object
    std::string currentSegment = "", itemSegment, itemTrait, itemOperation, translatedKey;

    std::regex segment_trait_key_regex {R"~((([a-z]*):*)([a-z]*):(.*))~"};
    std::smatch segment_trait_key_data;


    auto item = requestData.begin();
    while (!requestData.empty())
    {
        for (auto item = requestData.begin(); item != requestData.end(); )
        {
            if (!regex_match(item.key(), segment_trait_key_data, segment_trait_key_regex)) continue;
            if (segment_trait_key_data[3] == "")
            {
                itemOperation = "";
                itemSegment = segment_trait_key_data[2];
            }
            else
            {
                itemOperation = segment_trait_key_data[2];
                itemSegment = segment_trait_key_data[3];
            }
            itemTrait = segment_trait_key_data[4];

            if (currentSegment.empty())
            {
                currentSegment = itemSegment;
                build_open_tag(currentSegment);
                build_end_nested_tag();
            }

            if ((itemSegment.compare(currentSegment) == 0))
            {
                //Build each individual trait
                translatedKey = get_racf_key(
                    adminType.c_str(),
                    itemSegment.c_str(),
                    (itemSegment + ":" + itemTrait).c_str(),
                    TRAIT_TYPE_ANY,
                    OPERATOR_ANY
                );
                std::string operation = (itemOperation.empty()) ? "set" : itemOperation;
                std::string value = (item.value().is_boolean()) ? "" : json_value_to_string(item.value());
                build_single_trait(("racf:" + translatedKey), operation, value);
                item = requestData.erase(item);

            }
            else item++;
        }
        build_full_close_tag(currentSegment);
        currentSegment = "";
    }
}

std::string XmlGen::json_value_to_string(const nlohmann::json &j)
{
    if (j.type() == nlohmann::json::value_t::string) {
        return j.get<std::string>();
    }

    return j.dump();
}

std::string XmlGen::convert_operation(
    std::string requestOperation,
    int * irrsmo00_options
) {
    //Converts the designated function to the correct IRRSMO00 operation and adjusts IRRSMO00 options as necessary
    //(alter operations require the PRECHECK attribute)
    if (requestOperation.compare("add") == 0)  {
        return "set";
    }
    if (requestOperation.compare("alter") == 0) {
        *irrsmo00_options = 15;
        return "set";
    }
    if (requestOperation.compare("delete") == 0) {
        return "del";
    }
    if (requestOperation.compare("extract") == 0) {
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
