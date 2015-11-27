/*
 *
 gcc -D_REENTRANT test.c -I"include" -I. SAX.c entities.c encoding.c error.c  parserInternals.c parser.c tree.c hash.c list.c xmlIO.c  xmlmemory.c uri.c valid.c xlink.c HTMLparser.c HTMLtree.c  debugXML.c xpath.c xpointer.c xinclude.c nanohttp.c nanoftp.c  DOCBparser.c catalog.c globals.c threads.c c14n.c xmlstring.c  xmlregexp.c xmlschemas.c xmlschemastypes.c xmlunicode.c  xmlreader.c relaxng.c dict.c SAX2.c legacy.c chvalid.c pattern.c xmlsave.c xmlmodule.c schematron.c  -lpthread -lm -lz -ldl && ./a.out regressions.xml
 gcc -DHAVE_CONFIG_H -D_REENTRANT test.c -I"include" -I. SAX.c entities.c encoding.c error.c  parserInternals.c parser.c tree.c hash.c list.c xmlIO.c  xmlmemory.c uri.c valid.c xlink.c HTMLparser.c HTMLtree.c  debugXML.c xpath.c xpointer.c xinclude.c nanohttp.c nanoftp.c  DOCBparser.c catalog.c globals.c threads.c c14n.c xmlstring.c  xmlregexp.c xmlschemas.c xmlschemastypes.c xmlunicode.c  xmlreader.c relaxng.c dict.c SAX2.c xmlwriter.c legacy.c chvalid.c pattern.c xmlsave.c xmlmodule.c schematron.c triostr.c trio.c trionan.c -lpthread -lm -lz -ldl && ./a.out regressions.xml
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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


int main()
{
	char * xmls = 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<bookstore>"
		"<book>"
		"  <title lang=\"eng\">Harry Potter</title>"
		"  <price>29.99</price>"
		"</book>"
		"<book>"
		"  <title lang=\"eng\">Learning XML</title>"
		"  <price>39.95</price>"
		"</book>"
		"<book>"
		"  <title lang=\"en\">Harry</title>"
		"  <author>J K. Rowling</author> "
		"  <year>2005</year>"
		"  <price>29.99</price>"
		"</book>"
		"</bookstore>"
		;
	xmlNodePtr cur;  //������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�)
	xmlChar *key;
	xmlDocPtr doc = xmlReadMemory(xmls, strlen(xmls), NULL, NULL, 0);
	/*�������ĵ��Ƿ�ɹ���������ɹ���libxml��ָһ��ע��Ĵ���ֹͣ��һ�����������ǲ��ʵ��ı��롣XML��׼�ĵ�������UTF-8��UTF-16�⻹�����������뱣�档����ĵ���������libxml���Զ���Ϊ��ת����UTF-8���������XML������Ϣ������XML��׼�С�*/
	if (doc == NULL )
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		return -1;
	}
	cur = xmlDocGetRootElement(doc);  //ȷ���ĵ���Ԫ��
	/*���ȷ�ϵ�ǰ�ĵ��а�������*/
	if (cur == NULL)
	{
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return -2;
	}
	if (xmlStrcmp(cur->name, (const xmlChar *) "bookstore"))
	{
		fprintf(stderr,"document of the wrong type, root node != root");
		xmlFreeDoc(doc);
		return -3;
	}
	cur = cur->xmlChildrenNode;
	while(cur!=NULL)
	{
		xmlNodePtr son = cur->children;
		while(son){
			if ((!xmlStrcmp(son->name, (const xmlChar *)"price")))
			{
				key = xmlNodeListGetString(doc, son->xmlChildrenNode, 1);
				printf("keyword: %s\n", key);
				xmlFree(key);
			}
			son = son->next;
		}
		cur = cur->next;
	}








	//xmlChar *xpath = ("/bookstore/book[1]");
	//xmlChar *xpath = ("/bookstore/book");
	//xmlChar *xpath = ("/bookstore/book[price>35]/price");
	//xmlChar *xpath = ("/bookstore/book/title");
	//xmlChar *xpath = ("//title[@lang='eng']");
	xmlChar *xpath = ("//title[@lang]");
	xmlXPathObjectPtr app_result = get_nodeset(doc,xpath);
	if (app_result == NULL)
	{
		printf("app_result is NULL\n");
		return -4;
	}
	int i = 0;
	xmlChar *value;
	if(app_result)
	{
		xmlNodeSetPtr nodeset = app_result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++)
		{
			cur = nodeset->nodeTab[i];   
			printf("name: %s\n", (char *)cur->name);
			cur = cur->xmlChildrenNode; 
			while(cur!=NULL)
			{  
				value = xmlGetProp(cur,(const xmlChar *)"lang");
				if (value != NULL)
				{
					printf("xmlGetProp: %s\n", (char *)value);
					xmlFree(value);
				}
				value = xmlNodeGetContent(cur);
				if (value != NULL)
				{
					printf("xmlNodeGetContent: %s\n", (char *)value);
					xmlFree(value);
				}
				cur = cur->next;
			}

			printf("----------------\n");
		}
		xmlXPathFreeObject (app_result);
	}







	xmlFreeDoc(doc); 
	return 0;
}
