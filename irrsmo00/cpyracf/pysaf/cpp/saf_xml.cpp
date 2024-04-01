#include "../h/saf_xml.h"

//Private functions
void XmlGen::build_open_tag(string tag) {
    xml_buffer.append("<"+tag);
}
void XmlGen::build_attribute(string attribute) {
    xml_buffer.append(" "+attribute);
}
void XmlGen::build_value(string value) {
    xml_buffer.append(">"+value);
}
void XmlGen::build_end_nested_tag() {
    xml_buffer.append(">");
}
void XmlGen::build_full_close_tag(string tag) {
    xml_buffer.append("</"+tag+">");
}
void XmlGen::build_close_tag_no_value() {
    xml_buffer.append("/>");
}
void XmlGen::build_single_trait(string tag, string operation, string value) {
    build_open_tag(tag);
    if (operation.length() != 0) { build_attribute(make_xml_attribute("operation",operation)); }
    if (value.length() == 0) { build_close_tag_no_value(); }
    else {
        build_value(value);
        build_full_close_tag(tag);
    }
}
string XmlGen::make_xml_attribute(string name, string value)
{
    string output = name;
    output.append("=\""+value+"\"");
    return output;
}

void XmlGen::convert_to_ebcdic(char * ascii_str, int length){
    for(int i = 0; i < length; i++)
    {
        *(ascii_str+i) = EbcdicToAscii[(unsigned char)*(ascii_str+i)];
    }
}

char * XmlGen::build_xml_string(char * json_req_string, bool debug)
{
    nlohmann::json request;
    request = nlohmann::json::parse(json_req_string);
    string requestType;
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
        if ( item.key().compare("segments") == 0 ) { break; }
        build_attribute(item.key()+"=\""+item.value().get<string>()+"\"");
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
                string operation = (trait.value()["operation"].is_null()) ? "set" : trait.value()["operation"].get<string>();
                string value = (trait.value()["value"].is_boolean()) ? "" : trait.value()["value"].get<string>();
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

    if (debug)
    {
        cout << "XML Request String (Ascii): " << xml_buffer << "\n";
    }

    const int length = xml_buffer.length();
    char* output_buffer = new char[length + 1];
    strncpy(output_buffer, xml_buffer.c_str(), length);
    convert_to_ebcdic(output_buffer, length);

    if (debug)
    {
        cout << hex << "XML Request String (Ebcdic): " << hex << cast_hex_string(output_buffer) << "\n";
    }

    return output_buffer;
}

void XmlParse::parse_header_attributes(nlohmann::json * input_json, string header_string)
{
    smatch attr_sub_re_match;
    regex attr {R"~(([a-z]*)="([^ ]*)")~"};

    string::size_type n = 0, n_old;
    string sel_str;
    do
    {
        n_old = (n == 0) ? n:n+1;
        n = header_string.substr(n_old,header_string.length()-n_old).find(' ') + n_old;
        sel_str = header_string.substr(n_old,n - n_old);
        if (regex_match(sel_str, attr_sub_re_match, attr))
        {
            //cout << attr_sub_re_match[0] << '\n'; // Full String
            //cout << attr_sub_re_match[1] << '\n'; // Attribute Key
            //cout << attr_sub_re_match[2] << '\n'; // Attribute Value
            (*input_json)[attr_sub_re_match[1]] = attr_sub_re_match[2];
        }
    }while(n_old != n + 1);
};

void XmlParse::parse_outer_xml(nlohmann::json * input_json, string body_string)
{
    regex outer_tag {"<([a-z]*)>.*<\/([a-z]*)>"};
    smatch body_sub_re_match, body_iter_sub_re_match;
    
    string::size_type n = 0, n_old;
    string sel_str, current_tag, current_end_tag, remaining_str;

    if(regex_match(body_string, body_sub_re_match, outer_tag))
    {
        current_tag = body_sub_re_match[1];
        regex inner_tag {"<"+current_tag+">(.*?)<\/"+current_tag+">"};
        do
        {
            n_old = (n == 0) ? n:n+current_end_tag.length();
            current_end_tag = "<\/"+current_tag+">";
            regex inner_tag {"<"+current_tag+">(.*?)<\/"+current_tag+">"};
            n = body_string.substr(n_old,body_string.length()-n_old).find(current_end_tag) + n_old;
            sel_str = body_string.substr(n_old,n + current_end_tag.length() - n_old);
            //cout << "Selection String: " << sel_str << "\n";
            if (regex_match(sel_str, body_iter_sub_re_match, inner_tag))
            {
                //cout << "Full String: " << body_iter_sub_re_match[0] << '\n'; // Full String
                //cout << "Inner Tags: " << body_iter_sub_re_match[1] << '\n'; // Inner Tags
                parse_inner_xml(input_json, body_iter_sub_re_match[1], current_tag);

            }

            remaining_str = body_string.substr(n+current_end_tag.length(),body_string.length()-current_end_tag.length()-n);
            //cout << "Remaining String: " << remaining_str << "\n";
            if (regex_match(remaining_str, body_iter_sub_re_match, outer_tag))
            {
                current_tag = body_iter_sub_re_match[1];
                //cout << "Updating Current Tag: " << current_tag << "\n";
            }
        }while(n + current_end_tag.length() < body_string.length());
    }
};

void XmlParse::parse_inner_xml(nlohmann::json * input_json, string inner_data, string outer_tag)
{
    if (inner_data.find("<") == string::npos)
    {
        //cout << "Updating " << outer_tag << " with value: " << inner_data << "\n";
        update_json(input_json, inner_data, outer_tag);
        return;
    }
    nlohmann::json temp;
    parse_outer_xml(&temp, inner_data);
    update_json(input_json, temp, outer_tag);
}

void XmlParse::update_json(nlohmann::json * input_json, nlohmann::json inner_data, string outer_tag)
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

void XmlParse::convert_to_ascii(string * ebcdic_str)
{
    for(int i = 0; i < (*ebcdic_str).length(); i++)
    {
        (*ebcdic_str)[i] = AsciiToEbcdic[(unsigned char)(*ebcdic_str)[i]];
    }
}

char * XmlParse::build_json_string(char * xml_result_string, bool debug)
{
    xml_buffer = xml_result_string;

    if (debug)
    {
        cout << "XML Result String (Ebcdic): " << hex << cast_hex_string( xml_result_string ) << "\n";
    }

    convert_to_ascii(&xml_buffer);

    if (debug)
    {
        cout << "XML Result String (Ascii): " << xml_buffer << "\n";
    }

    smatch xml_sub_re_match;
    regex full_xml {R"~(<\?xml version="1\.0" encoding="IBM-1047"\?><securityresult xmlns="http:\/\/www\.ibm\.com\/systems\/zos\/saf\/IRRSMO00Result1"><([a-z]*) ([^>]*)>(<.+>)<\/securityresult>)~"}; 

    nlohmann::json result;
    nlohmann::json profile;

    string profile_type, profile_close_tag, profile_xml_attrs, profile_xml_body;

    if(regex_match(xml_buffer, xml_sub_re_match, full_xml))
    {
    //cout << xml_sub_re_match[0] << "\n\n"; // Full String
    //cout << xml_sub_re_match[1] << '\n'; // Profile Type
    //cout << xml_sub_re_match[2] << '\n'; // Profile Attrs
    //cout << xml_sub_re_match[3] << '\n'; // Command Information

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

    
    
    string json_result = result.dump();
    const int length = json_result.length();
    char* output_buffer = new char[length + 1];
    strncpy(output_buffer, json_result.c_str(), length);
    return output_buffer;
}

string cast_hex_string(char * input)
{
    string output = "{ ";
    for(int i = 0; i < strlen(input); i++)
    {
        if (i > 0) { output += ", "; }
        output += "x" + to_string((unsigned char)*(input+i));
    }
    output += " }";
    return output;
}