/* xml_conn.h */

#ifndef XML_C_CONNECTOR_H_
#define XML_C_CONNECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * injson_to_inxml(char * injson, char * userid_buffer,unsigned int * irrsmo00_options, unsigned int * result_buffer_size, bool * debug);
char * outxml_to_outjson(char * outxml, bool debug);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XML_C_CONNECTOR_H_ */