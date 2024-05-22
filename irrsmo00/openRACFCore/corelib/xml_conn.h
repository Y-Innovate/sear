/* xml_conn.h */

// Connects the "XML library" to the C layer with these extern C functions

#ifndef XML_C_CONNECTOR_H_
#define XML_C_CONNECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * injson_to_inxml(char * injson, char * userid_buffer,unsigned int * irrsmo00_options, unsigned int * result_buffer_size, bool * debug);
char * outxml_to_outjson(char * outxml, unsigned int saf_rc, unsigned int racf_rc, unsigned int racf_rsn, bool debug);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XML_C_CONNECTOR_H_ */