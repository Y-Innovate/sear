#include "saf_xml_parse.hpp"

#include <regex>
#include <string>
#include <iostream>
#include <unistd.h>

//Public Methods of XmlParse
char * XmlParse::build_json_string(
    char * xml_result_string,
    unsigned char opcode,
    int saf_rc,
    int racf_rc,
    int racf_rsn,
    bool debug
) {   
    //Build a JSON string from the XML result string, SMO return and Reason Codes, and the OPCODE used for the function
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

    std::string admin_type, admin_close_tag, admin_xml_attrs, admin_xml_body;

    if(regex_match(xml_buffer, xml_sub_re_match, full_xml))
    {
        //If we match the XML structure, we pull out relevant information with sub-matches.
        //std::cout << xml_sub_re_match[0] << "\n\n"; // Full std::string
        //std::cout << xml_sub_re_match[1] << '\n'; // Admin Type
        //std::cout << xml_sub_re_match[2] << '\n'; // Admin Attrs
        //std::cout << xml_sub_re_match[3] << '\n'; // Command Information

        //Use sub-matches in the regular expression to pull out useful information from the header of the xml
        admin_type = xml_sub_re_match[1];
        admin_xml_attrs = xml_sub_re_match[2];
        //Use a sub-match in the regular expression to identify the body of the xml for further parsing
        admin_xml_body = xml_sub_re_match[3];

        //Identify the closing tag of the xml for later xml operations
        admin_close_tag = R"(</)"+admin_type+">";

        //Parse out the header attributes
        parse_header_attributes(&result, admin_xml_attrs);
        //Erase the profile close tag as it messes up later regex parsing
        admin_xml_body.erase(admin_xml_body.find(admin_close_tag),admin_close_tag.length());
        //Parse the body of the xml here
        parse_outer_xml(&result, admin_xml_body);

        //Define attributes to the outer layer of the JSON
        result_json["adminType"] = admin_type;
        
        //Put the built JSON object in the result JSON
        result_json["result"] = result;
    }
    else
    {
        //If the XML does not match the main regular expression, then return this string to indicate an error
        result_json["error"] = "XML PARSE ERROR: Could not match data to valid xml patterns!";
    }
    //Return the type of operation requested as part of the JSON in success or failure case
    result_json["requestOperation"] = decode_opcode(opcode);

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

    //Return the char * c string
    return output_buffer;
}

//Private Methods of XmlParse
void XmlParse::parse_header_attributes(
    nlohmann::json * input_json,
    std::string header_string
) {
    //Parse the header attributes of the XML for JSON information
    std::smatch attr_sub_re_match;
    std::regex attr {R"~(([a-z]*)="([^ ]*)")~"};

    std::string::size_type n = 0, n_old;
    std::string sel_str;
    do
    {
        //Loop through XML attributes and add them to the input_json JSON object
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

void XmlParse::parse_outer_xml(
    nlohmann::json * input_json,
    std::string body_string
) {
    //Parse the outer layer of the XML for attributes and tag names with regex
    std::regex outer_tag {R"(<([a-z]*)>.*</([a-z]*)>)"};
    std::smatch body_sub_re_match, body_iter_sub_re_match;
    
    std::string::size_type n = 0, n_old;
    std::string sel_str, current_tag, current_end_tag, remaining_str;

    if(regex_match(body_string, body_sub_re_match, outer_tag))
    {
        //Use regex substrings to identify the name of the current xml tag
        current_tag = body_sub_re_match[1];
        std::regex inner_tag {"<"+current_tag+R"(>(.*?)</)"+current_tag+">"};
        do
        {
            //Enter a loop iterating through xml looking for XML tags within the "current" tag
            //In a practical sense, from SMO this ends up parsing "Command" entries, then looking
            //At individual xml entries within these "Command" entries like "image" or "message"
            n_old = (n == 0) ? n:n+current_end_tag.length();
            current_end_tag = R"(</)"+current_tag+">";
            std::regex inner_tag {"<"+current_tag+R"(>(.*?)</)"+current_tag+">"};
            n = body_string.substr(n_old,body_string.length()-n_old).find(current_end_tag) + n_old;
            sel_str = body_string.substr(n_old,n + current_end_tag.length() - n_old);
            //std::cout << "Selection std::string: " << sel_str << "\n";
            if (regex_match(sel_str, body_iter_sub_re_match, inner_tag))
            {
                //If we identified an inner XML object, so we call parse_inner_xml to parse it
                //std::cout << "Full std::string: " << body_iter_sub_re_match[0] << '\n'; // Full std::string
                //std::cout << "Inner Tags: " << body_iter_sub_re_match[1] << '\n'; // Inner XML object
                parse_inner_xml(input_json, body_iter_sub_re_match[1], current_tag);

            }

            //Check to see if I have finished parsing this "outer tag" (like a "command") entry
            //If so, get the next "outer tag" (possibly another "command" entry)
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

void XmlParse::parse_inner_xml(
    nlohmann::json * input_json,
    std::string inner_data,
    std::string outer_tag
) {
    //Parse data from within XML tags and add the values to the JSON
    if (inner_data.find("<") == std::string::npos)
    {
        //Confirmed there are no XML tags within this XML object, so just add the data to the JSON
        //std::cout << "Updating " << outer_tag << " with value: " << inner_data << "\n";
        update_json(input_json, inner_data, outer_tag);
        return;
    }
    //If we did not return, there is another xml tag within this data (nested)
    //Recursively call parse_outer_xml to handle the xml tags here, and create a new JSON to store this data
    nlohmann::json nested_json;
    parse_outer_xml(&nested_json, inner_data);
    //Add the JSON for the nested data to the main JSON
    update_json(input_json, nested_json, outer_tag);
}

void XmlParse::update_json(
    nlohmann::json * input_json,
    nlohmann::json inner_data,
    std::string outer_tag
) {
    //Add specified information (inner_data) to the input_json JSON object
    //using the specified key (outer_tag)
    if (!((*input_json).contains(outer_tag) || (*input_json).contains(outer_tag+"s")))
    {
        //If we do not already have this tag used in our object (at this layer), just add data
        (*input_json)[outer_tag] = inner_data;
        return;
    }
    if ((*input_json).contains(outer_tag))
    {
        //If we do already use this tag, pluralize the tag and merge the data
        (*input_json)[outer_tag+"s"] = {(*input_json)[outer_tag], inner_data};
        (*input_json).erase(outer_tag);
    }
    else
    {
        //If we already have the plural version of this tag, simply merge the data
        (*input_json)[outer_tag+"s"].push_back(inner_data);
    }
}

void XmlParse::convert_to_ascii(
    char * ebcdic_str,
    int length
) {
    //Universal function to convert EBCDIC-1047 string to ascii in place
    #ifndef __MVS__
    for(int i = 0; i < length; i++)
    {
        *(ebcdic_str+i) = EbcdicToAscii[(unsigned char)*(ebcdic_str+i)];
    }
    #else
    //If we are on z/OS, we use the built in e2a function for this
    __e2a_s(ebcdic_str);
    #endif //__MVS__
}

std::string XmlParse::decode_opcode(unsigned char opcode)
{
    //Decode the opcode into a string representing the designated function
    switch (opcode) {
        case OP_ADD:
            return "add";
        case OP_ALT:
            return "alter";
        case OP_DEL:
            return "delete";
        case OP_LST:
            return "extract";
        default:
            return "unsupported";
    }
}

// Connects the "XML library" to the C layer with this extern C function

extern char * outxml_to_outjson(
    char * outxml,
    unsigned char opcode,
    int saf_rc,
    int racf_rc,
    int racf_rsn,
    bool debug
) {
    //Build an XMLParse XML Parser object and parse an IRRSMO00
    //response xml string into a JSON string
    XmlParse * xml = new XmlParse();
    return xml->build_json_string(outxml, opcode, saf_rc, racf_rc, racf_rsn, debug);
}

#ifndef XML_COMMON_LIB_H_
//Functions common to both XML libraries

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
#endif /* XML_COMMON_LIB_H_ */