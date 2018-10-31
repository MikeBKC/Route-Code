/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

/* Parse text to JSON, then render back to text, and print! */
void doit(char *text)
{
	char *out;cJSON *json;
	cJSON *item;
	int fmt = 1, depth = 0;
	
	json=cJSON_Parse(text);
	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
	else
	{
		out=cJSON_Print(json);
		printf("%s\n",out);
		printf("out =%s\n",json->child->string);
		if((item = cJSON_GetObjectItem(json->child, "IDs"))!=NULL) {
#if 0		    
		    out = 0;
		    switch ((item->type)&255)
		    {
			case cJSON_NULL:	out=cJSON_strdup("null");	break;
			case cJSON_False:	out=cJSON_strdup("false");break;
			case cJSON_True:	out=cJSON_strdup("true"); break;
			case cJSON_Number:	out=print_number(item);break;
			case cJSON_String:	out=print_string(item);break;
			case cJSON_Array:	out=print_array(item,depth,fmt);break;
			case cJSON_Object:	out=print_object(item,depth,fmt);break;
		    }
#endif
		    printf("type =%d\n",item->type);
		    if(item->type == 5) {
			if(item->child != NULL) {
			    printf("type =%d\n",item->child->type);
			    printf("double =%ld\n",item->child->valuedouble);
			    printf("int =%d\n",item->child->valueint);
			    if(item->child->next != NULL) {
				printf("type =%d\n",item->child->next->type);
			    printf("int =%d\n",item->child->next->valueint);
			    }
			    
			}
		    }
		    printf("out =%s\n",item->string);
		}
		item = NULL;
		if((item = cJSON_GetObjectItem(json->child, "Title"))!=NULL) {
		    printf("type =%d\n",item->type);
		    printf("string =%s\n",item->string);
		    printf("valuestring =%s\n",item->valuestring);
		}

		free(out);
		cJSON_Delete(json);
	}
}

void 
do_test()
{
    /*
     *      {"mac":["00-e0-66-29-0b-ab","90-18-7c-96-f0-c6"],"state":"1","timestamp":"20140808110711"}
     *      345          *      {"mac":[],"state":"1","timestamp":"20140808110711"}
     *      346          *      */
    //char text[] = "{\"mac\":[\"00-e0-66-29-0b-ab\",\"90-18-7c-96-f0-c6\"],\"state\":\"1\",\"timestamp\":\"20140808110711\"}";
    char text[] = "{\"mac\":[],\"state\":\"1\",\"timestamp\":\"20140808110711\"}";
    char *out;
    cJSON *json;
    cJSON *item;
    cJSON *macP;
    
    json=cJSON_Parse(text);
    if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
    else
    {
	out=cJSON_Print(json);
	printf("%s\n",out);
	printf("type =%d\n",json->type);

	if((item = cJSON_GetObjectItem(json, "state"))!=NULL) {
	    printf("type =%d\n",item->type);
	    printf("sting =%s\n",item->string);
	    printf("valuestring =%s\n",item->valuestring);
	    /*state == "1" ,indicate that UTT Server is online*/
	    if(strcmp(item->valuestring, "1") == 0) {
	    
	    }
	    if((item = cJSON_GetObjectItem(json, "mac"))!=NULL) {
		printf("mac type =%d\n",item->type);
		if(item->child != NULL) {
		    macP = item->child;
		    while(macP!=NULL) {
		    printf("mac child type =%d\n",macP->type);
		    printf("mac child string =%s\n",macP->string);
		    if(macP->valuestring != NULL) { 
			/*need cown mac */
			printf("mac child valuestring =%s\n",macP->valuestring);
		    }
			/* get the next need down mac */
			macP = macP->next;
		    }
#if 0
		    macP = item->child;
		    if(macP->child != NULL) {
			printf("macP child type =%d\n",macP->child->type);
		    } else {
			printf("macP child null.\n");
		    }
#endif
		}
#if 0
		if(item->next != NULL) {
		    printf("next type =%d\n",item->next->type);
		    printf("next string =%s\n",item->next->string);
		    printf("next valuestring =%s\n",item->next->valuestring);
		} else {
		    printf("next type =null\n");
		}
#endif
	    }
	    
	} else {
	    /* no 'state' reurned ,means that UTT Server is offline */
	}

	free(out);
	cJSON_Delete(json);
    } 
}

void 
do_auth()
{
    /*
     * Auth 响应的JSON内容 
     * state替代Auth 1表示通过，0失败
     * {"expire":"120","state":"1","timestamp":"20140808111701"}
     * {"state":"0","timestamp":"20140808111701"}
     *      */
    char text[] = "{\"expire\":\"120\",\"state\":\"1\",\"timestamp\":\"20140808111701\"}";
    char *out;
    cJSON *json;
    cJSON *item;
    cJSON *macP;
    
    json=cJSON_Parse(text);
    if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
    else
    {
	out=cJSON_Print(json);
	printf("%s\n",out);
	printf("type =%d\n",json->type);

	if((item = cJSON_GetObjectItem(json, "state"))!=NULL) {
	    printf("type =%d\n",item->type);
	    printf("sting =%s\n",item->string);
	    printf("valuestring =%s\n",item->valuestring);
	    /*state == "1" ,indicate that UTT Server is online*/
	    if(strcmp(item->valuestring, "1") == 0) {
		printf("state= %s\n",item->valuestring);
		if((item = cJSON_GetObjectItem(json, "expire"))!=NULL) {
		    printf("expire type =%d\n",item->type);
		    printf("expire string =%s\n",item->string);
		    printf("expire valuestring =%s, %d\n",item->valuestring,atoi(item->valuestring));
#if 1
		    if(item->next != NULL) {
			printf("next type =%d\n",item->next->type);
			printf("next string =%s\n",item->next->string);
			printf("next valuestring =%s\n",item->next->valuestring);
		    } else {
			printf("next type =null\n");
		    }
#endif
		} else {
		    /*默认取值20分钟*/
		}
	    } else {
		    /*默认取值20分钟*/
	    printf("state= %s\n",item->valuestring);
	    }
	    
	} else {
	    /* no 'state' reurned ,means that UTT Server is offline */
	}

	free(out);
	cJSON_Delete(json);
    } 
}
/* Read a file, parse, render back, etc. */
void dofile(char *filename)
{
	FILE *f=fopen(filename,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
	doit(data);
	free(data);
}

/* Used by some code below as an example datatype. */
struct record {const char *precision;double lat,lon;const char *address,*city,*state,*zip,*country; };

/* Create a bunch of objects as demonstration. */
void create_objects()
{
	cJSON *root,*fmt,*img,*thm,*fld;char *out;int i;	/* declare a few. */

	/* Here we construct some JSON standards, from the JSON site. */
	
	/* Our "Video" datatype: */
	root=cJSON_CreateObject();	
	cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
	cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());
	cJSON_AddStringToObject(fmt,"type",		"rect");
	cJSON_AddNumberToObject(fmt,"width",		1920);
	cJSON_AddNumberToObject(fmt,"height",		1080);
	cJSON_AddFalseToObject (fmt,"interlace");
	cJSON_AddNumberToObject(fmt,"frame rate",	24);
	
	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */

	/* Our "days of the week" array: */
	const char *strings[7]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	root=cJSON_CreateStringArray(strings,7);

	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);

	/* Our matrix: */
	int numbers[3][3]={{0,-1,0},{1,0,0},{0,0,1}};
	root=cJSON_CreateArray();
	for (i=0;i<3;i++) cJSON_AddItemToArray(root,cJSON_CreateIntArray(numbers[i],3));

/*	cJSON_ReplaceItemInArray(root,1,cJSON_CreateString("Replacement")); */
	
	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);


	/* Our "gallery" item: */
	int ids[4]={116,943,234,38793};
	root=cJSON_CreateObject();
	cJSON_AddItemToObject(root, "Image", img=cJSON_CreateObject());
	cJSON_AddNumberToObject(img,"Width",800);
	cJSON_AddNumberToObject(img,"Height",600);
	cJSON_AddStringToObject(img,"Title","View from 15th Floor");
	cJSON_AddItemToObject(img, "Thumbnail", thm=cJSON_CreateObject());
	cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
	cJSON_AddNumberToObject(thm,"Height",125);
	cJSON_AddStringToObject(thm,"Width","100");
	cJSON_AddItemToObject(img,"IDs", cJSON_CreateIntArray(ids,4));

	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);

	/* Our array of "records": */
	struct record fields[2]={
		{"zip",37.7668,-1.223959e+2,"","SAN FRANCISCO","CA","94107","US"},
		{"zip",37.371991,-1.22026e+2,"","SUNNYVALE","CA","94085","US"}};

	root=cJSON_CreateArray();
	for (i=0;i<2;i++)
	{
		cJSON_AddItemToArray(root,fld=cJSON_CreateObject());
		cJSON_AddStringToObject(fld, "precision", fields[i].precision);
		cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
		cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
		cJSON_AddStringToObject(fld, "Address", fields[i].address);
		cJSON_AddStringToObject(fld, "City", fields[i].city);
		cJSON_AddStringToObject(fld, "State", fields[i].state);
		cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
		cJSON_AddStringToObject(fld, "Country", fields[i].country);
	}
	
/*	cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root,1),"City",cJSON_CreateIntArray(ids,4)); */
	
	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);

}

int main (int argc, const char * argv[]) {
	/* a bunch of json: */
	char text1[]="{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";	
	char text2[]="[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";
	char text3[]="[\n    [0, -1, 0],\n    [1, 0, 0],\n    [0, 0, 1]\n	]\n";
	char text4[]="{\n		\"Image\": {\n			\"Width\":  800,\n			\"Height\": 600,\n			\"Title\":  \"View from 15th Floor\",\n			\"Thumbnail\": {\n				\"Url\":    \"http:/*www.example.com/image/481989943\",\n				\"Height\": 125,\n				\"Width\":  \"100\"\n			},\n			\"IDs\": [116, 943, 234, 38793]\n		}\n	}";
	char text5[]="[\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.7668,\n	 \"Longitude\": -122.3959,\n	 \"Address\":   \"\",\n	 \"City\":      \"SAN FRANCISCO\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94107\",\n	 \"Country\":   \"US\"\n	 },\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.371991,\n	 \"Longitude\": -122.026020,\n	 \"Address\":   \"\",\n	 \"City\":      \"SUNNYVALE\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94085\",\n	 \"Country\":   \"US\"\n	 }\n	 ]";

	/* Process each json textblock by parsing, then rebuilding: */
	//doit(text1);
	//doit(text2);	
	//doit(text3);
	//doit(text4);
	//doit(text5);
	//do_test();
	do_auth();

	/* Parse standard testfiles: */
/*	dofile("../../tests/test1"); */
/*	dofile("../../tests/test2"); */
/*	dofile("../../tests/test3"); */
/*	dofile("../../tests/test4"); */
/*	dofile("../../tests/test5"); */

	/* Now some samplecode for building objects concisely: */
	//create_objects();
	
	return 0;
}
