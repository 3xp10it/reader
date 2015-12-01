/**
 * @file xlsx.c
 gcc -Wall -I"xml2" -lxml2  mystring.c -lm -D test_xlsx -D STDC_HEADERS xlsx.c zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c && ./a.out
 gcc -I"xml2" -lxml2  myregex.c mystring.c regex.c -lm -D test_xlsx -D STDC_HEADERS xlsx.c zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-11-02
 */

#include "zip.h"
//#include "myregex.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	context = xmlXPathNewContext(doc);
	if (context == NULL)
	{
		printf("context is NULL\n");
		return NULL;
	}
	xmlXPathRegisterNs(context,(const xmlChar*)"ns",(const xmlChar*)"http://schemas.openxmlformats.org/spreadsheetml/2006/main");//  //Ĭ��ns :   @"//*[local-name()='price'] " 
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL)
	{
		printf("xmlXPathEvalExpression return NULL\n");
		return NULL;
	}
	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		printf("nodeset is empty\n");
		return NULL;
	}
	return result;
}

#ifdef test_xlsx
int main(int argc,char **argv)
{
	char * filename;
	if(argc>1)
	{
		filename = argv[argc-1];
	}else{
		filename = "xls.xlsx";
	}

	FILE * _file = fopen(filename,"rb");
	int fileLen = fseek(_file,0,SEEK_END);
	fileLen = ftell(_file);
	rewind(_file);
	printf("%s,%d\n",filename,fileLen);

	ByteArray * bytearray = ByteArray_new(fileLen);
	fread(bytearray->data,1,fileLen,_file);
	fclose(_file);

	char * out = malloc(fileLen*10);
	memset(out,0,fileLen*10);
	int outlen=0;
	//ZipFile_free(ZipFile_parser(bytearray,"",out,&outlen));

	ZipFile * zipfile = ZipFile_parser(bytearray,"xl/sharedStrings.xml",out,&outlen);
	//printf("\n%s",out);
	ZipFile_free(zipfile);

	/*
	   char **matched_arr=(char**)malloc(strlen(out));
	   memset(matched_arr,0,strlen(out));
	//<w:t></w:t>
	int len = regex_search_all(out,"/<t>([^<>]*)<\\/t>/",matched_arr);
	printf("%d\n",len);
	char * connected = string_concat(matched_arr,len,"");
	printf("%s",regex_replace_all(connected,"/<[\\/]*w:t>/",""));
	fflush(stdout);
	*/
	xmlDocPtr string_doc = xmlParseMemory(out, strlen(out));
	if (string_doc== NULL )
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		return -1;
	}

	xmlChar *xpath1 = (xmlChar*)("//ns:t");
	xmlXPathObjectPtr _result = get_nodeset(string_doc,xpath1);
	if (_result == NULL)
	{
		printf("_result is NULL\n");
		return -4;
	}
	int i;
	i = 0;
	xmlChar ** str_arr=NULL;
	if(_result)
	{
		xmlNodeSetPtr nodeset = _result->nodesetval;
		str_arr = malloc(sizeof(char**)*nodeset->nodeNr);
		memset(str_arr,0,sizeof(char**)*nodeset->nodeNr);
		for (i=0; i < nodeset->nodeNr; i++)
		{
			xmlNodePtr cur;  //������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�)
			cur = nodeset->nodeTab[i];   
			//printf("name: %s\n", (char *)cur->name);
			cur = cur->xmlChildrenNode; 
			while(cur!=NULL)
			{  
				xmlChar *value;
				value = xmlNodeGetContent(cur);
				str_arr[i]=value;
				if (value != NULL)
				{
					//printf("%s\t", (char *)value);
					//xmlFree(value);
				}
				cur = cur->next;
			}
			//printf("\r\n");
		}
		xmlXPathFreeObject (_result);
	}

	xmlFreeDoc(string_doc); 






	int sheetIndex = 0;
	while(1){
		sheetIndex++;
		memset(out,0,fileLen*10);
		// xl/worksheets/sheet1.xml
		char sheetName[64];
		memset(sheetName,0,64);
		sprintf(sheetName,"xl/worksheets/sheet%d.xml",sheetIndex);
		bytearray->position=0;
		ZipFile_free(ZipFile_parser(bytearray,sheetName,out,&outlen));
		//printf("\n%s\n",out);
		if(out==NULL || strlen(out)==0)
			break;

		xmlDocPtr doc = xmlParseMemory(out, strlen(out));
		if (doc == NULL )
		{
			fprintf(stderr,"Document not parsed successfully. \n");
			return -1;
		}

		xmlChar *xpath = (xmlChar*)("//ns:row");
		xmlXPathObjectPtr app_result = get_nodeset(doc,xpath);
		if (app_result == NULL)
		{
			printf("app_result is NULL\n");
			return -4;
		}
		if(app_result)
		{
			xmlNodeSetPtr nodeset = app_result->nodesetval;
			xmlNodePtr cur = nodeset->nodeTab[0];   
			while(cur)
			{
				//printf("name: %s\n", (char *)cur->name);
				xmlNodePtr son = cur->xmlChildrenNode;
				while(son)
				{  
					xmlChar *value;
					xmlChar* prop = xmlGetProp(son, (const xmlChar*)"t");  
					//if(prop) printf("prop:%s",prop);
					value = xmlNodeGetContent(son);
					if (value != NULL)
					{
						if(prop && strcmp("s",(char*)prop)==0)
						{
							//printf("====%s\t", (char *)value);
							printf("%s\t", (char *)str_arr[atoi((char*)value)]);
						}else{
							printf("%s\t", (char *)value);
						}
						xmlFree(value);
					}
					son = son->next;
					xmlFree(prop); 
				}
				printf("\r\n");
				cur = cur->next;
			}
			printf("\r\n");
			xmlXPathFreeObject (app_result);
		}
		xmlFreeDoc(doc); 
	}



	/*
	   xmlNodePtr cur;  //������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�)
	   cur = xmlDocGetRootElement(doc);  //ȷ���ĵ���Ԫ��
	   if (cur == NULL)
	   {
	   fprintf(stderr,"empty document\n");
	   xmlFreeDoc(doc);
	   return -2;
	   }
	   if (xmlStrcmp(cur->name, (const xmlChar *) "worksheet"))
	   {
	   xmlFreeDoc(doc);
	   return -3;
	   }
	   cur = cur->xmlChildrenNode;
	   while(cur!=NULL)
	   {
	   xmlNodePtr son = cur->children;
	   while(son){
		//xmlChar* prop = xmlGetProp(son, "spans");  
		//printf("\n%s,%s",son->name,prop);

		xmlChar *key;
		if ((!xmlStrcmp(son->name, (const xmlChar *)"sheetData")))
		{
		key = xmlNodeListGetString(doc, son->xmlChildrenNode, 1);
		printf("keyword: %s\n", key);
		xmlFree(key);
		}
		son = son->next;
		}
		cur = cur->next;
		}
		*/





		return 0;
		//regex_matchedarrClear(matched_arr,len);
}
#endif
