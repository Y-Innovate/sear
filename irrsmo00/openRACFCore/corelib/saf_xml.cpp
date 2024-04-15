#include "saf_xml.h"

#include <regex>
#include <string>
#include <iostream>

//Private functions
void XmlGen::build_open_tag(std::string tag) {
    xml_buffer.append("<"+tag);
}
void XmlGen::build_attribute(std::string attribute) {
    xml_buffer.append(" "+attribute);
}
void XmlGen::build_value(std::string value) {
    xml_buffer.append(">"+value);
}
void XmlGen::build_end_nested_tag() {
    xml_buffer.append(">");
}
void XmlGen::build_full_close_tag(std::string tag) {
    xml_buffer.append("</"+tag+">");
}
void XmlGen::build_close_tag_no_value() {
    xml_buffer.append("/>");
}
void XmlGen::build_single_trait(std::string tag, std::string operation, std::string value) {
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
    std::string output = name;
    output.append("=\""+value+"\"");
    return output;
}

void XmlGen::convert_to_ebcdic(char * ascii_str, int length){
    for(int i = 0; i < length; i++)
    {
        *(ascii_str+i) = EbcdicToAscii[(unsigned char)*(ascii_str+i)];
    }
}

char * XmlGen::build_xml_string(char * json_req_string, char * userid_buffer, unsigned int * irrsmo00_options, unsigned int * result_buffer_size, bool * debug)
{
    nlohmann::json request;
    request = nlohmann::json::parse(json_req_string);
    std::string requestType;
    //Build the securityrequest tag (Consistent)
    build_open_tag("securityrequest");
    build_attribute(make_xml_attribute("xmlns","http://www.ibm.com/systems/zos/saf"));
    build_attribute(make_xml_attribute("xmlns:racf","http://www.ibm.com/systems/zos/racf"));
    build_end_nested_tag();

    for (const auto& item : request.items()) { requestType = item.key(); }
    build_open_tag(requestType);

    //Build the admin object
    for (const auto& item : request[requestType].items())
    {
        if ( item.key().compare("segments") == 0 ) { continue; }
        if ( item.key().compare("runninguserid") == 0 )
        { 
            const int userid_length = item.value().get<std::string>().length();
            strncpy(userid_buffer, item.value().get<std::string>().c_str(), userid_length);
            convert_to_ebcdic(userid_buffer, userid_length);
            continue; 
        }
        if ( item.key().compare("irrsmo00options") == 0 )
        { 
            *irrsmo00_options = item.value().get<uint>();
            continue;
        }
        if ( item.key().compare("resultbuffersize") == 0 )
        { 
            *result_buffer_size = item.value().get<uint>();
            continue;
        }
        if ( item.key().compare("debugmode") == 0 )
        { 
            *debug = item.value().get<bool>();
            continue;
        }
        if ( item.value().is_string() )
        {
        build_attribute(item.key()+"=\""+item.value().get<std::string>()+"\"");
        }
    }
    if (request[requestType].contains("segments"))
    {
        build_end_nested_tag();

        //Build the segments
        for (const auto& item : request[requestType]["segments"].items())
        {
            build_open_tag(item.key());
            build_end_nested_tag();
            //Build each individual trait
            for (const auto& trait : request[requestType]["segments"][item.key()].items())
            {
                std::string operation = (trait.value()["operation"].is_null()) ? "set" : trait.value()["operation"].get<std::string>();
                std::string value = (trait.value()["value"].is_boolean()) ? "" : trait.value()["value"].get<std::string>();
                build_single_trait(trait.key(), operation, value);
            }
            build_full_close_tag(item.key());
        }
        
        //Close the admin object
        build_full_close_tag(requestType);
    }
    else
    {
        //Close the admin object
        build_close_tag_no_value();
    }
    
    //Close the securityrequest tag (Consistent)
    build_full_close_tag("securityrequest");

    if (*debug)
    {
        std::cout << "XML Request string (Ascii): " << xml_buffer << "\n";
    }

    const int length = xml_buffer.length();
    char* output_buffer = new char[length + 1];
    strncpy(output_buffer, xml_buffer.c_str(), length+1);
    convert_to_ebcdic(output_buffer, length);

    if (*debug)
    {
        std::cout << std::hex << "XML Request string (Ebcdic): " << std::hex << cast_hex_string(output_buffer) << "\n";
    }

    return output_buffer;
}

void XmlParse::parse_header_attributes(nlohmann::json * input_json, std::string header_string)
{
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
    std::regex outer_tag {"<([a-z]*)>.*<\/([a-z]*)>"};
    std::smatch body_sub_re_match, body_iter_sub_re_match;
    
    std::string::size_type n = 0, n_old;
    std::string sel_str, current_tag, current_end_tag, remaining_str;

    if(regex_match(body_string, body_sub_re_match, outer_tag))
    {
        current_tag = body_sub_re_match[1];
        std::regex inner_tag {"<"+current_tag+">(.*?)<\/"+current_tag+">"};
        do
        {
            n_old = (n == 0) ? n:n+current_end_tag.length();
            current_end_tag = "<\/"+current_tag+">";
            std::regex inner_tag {"<"+current_tag+">(.*?)<\/"+current_tag+">"};
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

void XmlParse::convert_to_ascii(std::string * ebcdic_str)
{
    for(int i = 0; i < (*ebcdic_str).length(); i++)
    {
        (*ebcdic_str)[i] = AsciiToEbcdic[(unsigned char)(*ebcdic_str)[i]];
    }
}

char * XmlParse::build_json_string(char * xml_result_string, unsigned int saf_rc, unsigned int racf_rc, unsigned int racf_rsn, bool debug)
{
    xml_buffer = xml_result_string;

    if (debug)
    {
        std::cout << "XML Result string (Ebcdic): " << std::hex << cast_hex_string( xml_result_string ) << "\n";
    }

    convert_to_ascii(&xml_buffer);

    if (debug)
    {
        std::cout << "XML Result string (Ascii): " << xml_buffer << "\n";
    }

    std::smatch xml_sub_re_match;
    std::regex full_xml {R"~(<\?xml version="1\.0" encoding="IBM-1047"\?><securityresult xmlns="http:\/\/www\.ibm\.com\/systems\/zos\/saf\/IRRSMO00Result1"><([a-z]*) ([^>]*)>(<.+>)<\/securityresult>)~"}; 

    nlohmann::json result;
    nlohmann::json profile;
    nlohmann::json returnCodes;

    std::string profile_type, profile_close_tag, profile_xml_attrs, profile_xml_body;

    if(regex_match(xml_buffer, xml_sub_re_match, full_xml))
    {
    //std::cout << xml_sub_re_match[0] << "\n\n"; // Full std::string
    //std::cout << xml_sub_re_match[1] << '\n'; // Profile Type
    //std::cout << xml_sub_re_match[2] << '\n'; // Profile Attrs
    //std::cout << xml_sub_re_match[3] << '\n'; // Command Information

    profile_type = xml_sub_re_match[1];
    profile_close_tag = "<\/"+profile_type+">";
    profile_xml_attrs = xml_sub_re_match[2];
    profile_xml_body = xml_sub_re_match[3];

    parse_header_attributes(&profile, profile_xml_attrs);
    profile_xml_body.erase(profile_xml_body.find(profile_close_tag),profile_close_tag.length());
    parse_outer_xml(&profile, profile_xml_body);
    result[xml_sub_re_match[1]] = profile;
    }
    else
    {
    result["error"] = "XML PARSE ERROR: Could not match data to valid xml patterns!";
    }

    returnCodes["safReturnCode"] = saf_rc;
    returnCodes["racfReturnCode"] = racf_rc;
    returnCodes["racfReasonCode"] = racf_rsn;

    result["returnCodes"] = returnCodes;
    
    std::string json_result = result.dump();
    const int length = json_result.length();
    char* output_buffer = new char[length + 1];
    strncpy(output_buffer, json_result.c_str(), length+1);
    return output_buffer;
}

std::string cast_hex_string(char * input)
{
    std::string output = "{ ";
    char buff[4];
    for(int i = 0; i < strlen(input); i++)
    {
        if (i > 0) { output += ", "; }
        std::snprintf(buff, 4, "x%02x", (unsigned char)*(input+i));
        output += buff;
    }
    output += " }";
    return output;
}