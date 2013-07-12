﻿#include "../cgitest/libcgi.h"
#include "../../tinyxml/tinyxml.h"

char_t DATA_DIR[] = L"\\data";
char_t CONFIG_FILE[] = L"config.xml";


#define strncpy wcsncpy
#define strlen wcslen
#define strcmp wcscmp
#define strncat wcsncat
#define print wprintf

static wchar_t *getStoken(wchar_t **str);
static void ListCurDir(char_t *dir);
//action=list
void gListFiles()
{
	ListCurDir(DATA_DIR);
}

//action=edit
// depend on argv !!! danger to move other file
// // nameBuf=a.xml|
//  we only get first xml
void gEditFile(wchar_t *nameBuf)
{
    wchar_t path[256];
    wchar_t buf[256];

    wchar_t *srcFile = getStoken(&nameBuf);
    write_data(L"<!doctype html>\n");
    write_data(L"<html lang=\"en\">\n");
    write_data(L"<head>\n");
    write_data(L"   <meta charset=\"UTF-8\" />\n");
    if(srcFile)
        write_data(L"   <title>编辑</title>\n");
    else
        write_data(L"   <title>新建</title>\n");

    write_data(L"   <style type=\"text/css\">\n");
    write_data(L"   input{width:80px;};\n");
    write_data(L"   label{width:80px;}\n");
    write_data(L"   </style>\n");
    write_data(L"   <script language=\"javascript\" src=\"../xml.js\">   \n");
    write_data(L"    </script>\n");
    write_data(L"</head>\n");
    write_data(L"<body>\n");
    write_data(L"           <table id=\"tab\">\n");
    write_data(L"                   <tr>\n");
    write_data(L"                           <th>\n");
    write_data(L"                                   <label>序号</label></th>\n");
    write_data(L"                           <th>\n");
    write_data(L"                                   <label>类型</label></th>\n");
    write_data(L"                           <th>\n");
    write_data(L"                                   <label>X</label></th>\n");
    write_data(L"                           </th>\n");
    write_data(L"                           <th>\n");
    write_data(L"                                   <label>Y</label></th>\n");
    write_data(L"                           <th>\n");
    write_data(L"                                   <label>预览</label></th>\n");
    write_data(L"                           </th>                                   \n");
    write_data(L"                           <th>\n");
    write_data(L"                                   <label>操作</label>\n");
    write_data(L"                           </th>\n");
    write_data(L"                   </tr>\n");

    //打开现有文件，解析xml到html
    DBG(L"file=%s",srcFile);
    if(srcFile){
        swprintf(path,L"%s\\%s",DATA_DIR,srcFile);
        int written=0;
        char toHtml[1000];
        int id =0 ;
        TiXmlDocument doc(UnicodeToAnsi(path));
        bool loadOkay = doc.LoadFile();

        if ( !loadOkay )
        {
            DBG(L"Could not load xml file. Error='%s'. Exiting.", doc.ErrorDesc() );
            exit( 1 );
        }
        TiXmlHandle docH( &doc );
        TiXmlNode* node = 0;
        TiXmlElement* todoElement = 0;
        TiXmlElement* itemElement = 0;
        itemElement = docH.FirstChildElement( "Production" ).FirstChildElement().ToElement();

        while(itemElement!= NULL){
            int x=0,y=0;
            const char *font,*fontsize,*text;
            itemElement->Attribute( "x" ,&x);
            itemElement->Attribute( "y" ,&y);
                // font = itemElement->Attribute( "font");
                // fontsize = itemElement->Attribute( "fontsize");
            text = itemElement->GetText();

            write_data(L"                   <tr>\n");
            write_data(L"                           <th><label>%d</label></th>\n",++id);
            write_data(L"                           <th><select name=\"\" id=\"\"><option value=\"text\">文本</option><option value=\"date\">日期</option><option value=\"time\">时间</option><option value=\"count\">计数器</option><option value=\"picture\">图片</option></select></th>\n");
            write_data(L"                           <th><input type=\"text\" name=\"x\" value=%d></th>\n",x);
            write_data(L"                           <th><input type=\"text\" name=\"y\" value=%></th>\n",y);
            write_data(L"                           <th><input type=\"text\" name=\"\" id=\"\" value=\"%s\"></th>\n",text);
            write_data(L"                           <th><input type=\"button\" value=\"删除字段\" onClick=\"delRow(this)\"></th>\n");
            write_data(L"                   </tr>\n");
            itemElement = itemElement->NextSiblingElement();
        }
    }

    write_data(L"           </table>\n");
    write_data(L"           <fieldset>\n");
    write_data(L"                   <p>\n");
    write_data(L"                           <input type=\"button\" value=\"添加字段\" onClick=\"addRow()\">\n");
    write_data(L"                           <label for=\"\">文件名:</label>\n");
    write_data(L"                           <input type=\"text\" id='file' value=\"app\">\n");
    write_data(L"                           <input type=\"submit\" value=\"提交修改\" onClick='doXML(0)'>\n");
    write_data(L"                           <input type=\"submit\" value=\"删除文件\" onClick='doXML(1)'></p>\n");
    write_data(L"           </fieldset>\n");
    write_data(L"</body>\n");
    write_data(L"</html>\n");
}

//action=save
//utf-8 xml file
void gSaveFile(wchar_t *srcFile,wchar_t *nameBuf)
{
	BOOL ret;
    // path UTF16 and UTF8
    char_t pathW[256];
    char *path;

    //xml content UTF16 and UTF8
    char_t *ptr;
    char *buf;

    int len,rlen;

    //UTF16 error desc
    wchar_t *to;

    swprintf(pathW,L"%s\\%s",DATA_DIR,nameBuf);
    //DBG(L"save path=%s",pathW);
    UnicodeToAnsi2(pathW,&path);

    TiXmlDocument doc;
    ptr = getCgiDataWithMalloc(&len);
    //DBG(ptr);
    //DBG(L"len=%d",len);
    UnicodeToAnsi2(ptr,&buf);
    //hexdump(ptr,len);
    //hexdump(buf,len * 2);

   if(ptr&&buf){//need to utf-8
    doc.Parse(buf, 0, TIXML_DEFAULT_ENCODING);
    //AnsiToUnicode(doc.ErrorDesc(),&to);
    //DBG(L"doc.parse:%s",to);
    doc.SaveFile(path);  
    AnsiToUnicode(doc.ErrorDesc(),&to);
    DBG(L"doc.parse:%s",to);
    free(to);
}else{
    DBG(L"save error");
}
free(path);
freeCgiDataWithMalloc(ptr);
free(buf);
}

//action=new
void gNewFile()
{
    gEditFile(NULL);
}

//action=delete
// nameBuf=a.xml|b.xml|...
void gDeleteFile(wchar_t *nameBuf)
{
    wchar_t *pos;
    while( (pos = getStoken(&nameBuf)) && (strlen(pos) > 0) ){
        char_t path[256];
        swprintf(path,L"%s\\%s",DATA_DIR,pos);
        DBG(L"del,%s",path);
        DeleteFile(path);
    }
}
//action=copy
// nameBuf=a.xml|b.xml|...
void gCopyFile(wchar_t *nameBuf)
{
}

//action=print
void gPrintFile(wchar_t *nameBuf)
{
}

//action=settings
void gSettings()
{

}

//action=home
void gHome()
{
	write_data(L"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>");
	write_data(L"<html xmlns='http://www.w3.org/1999/xhtml'>");
	write_data(L"<head>");
	write_data(L"	<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />");
	write_data(L"	<title>脰梅脪鲁</title>");
	write_data(L"</head>");
	write_data(L"<body>");
	write_data(L"	<form action='cgi-bin/test' method='get'>");
	write_data(L"		<p align='center'>");
	write_data(L"			<input name='textFilename' type='text' value='碌卤脟掳脦脛录镁脙没' />");
	write_data(L"		</p>");
	write_data(L"		<p align='center'>");
	write_data(L"			<input name='preview' type='image' value='脭陇脌脌' src='../preview.jpg' />");
	write_data(L"		</p>");
	write_data(L"		<p align='center'>");
	write_data(L"			<input name='bPrint' type='button' value='麓貌脫隆' onclick='window.location.href=print.html'/>");
	write_data(L"			<input name='bManage' type='button' value='鹿脺脌铆' />");
	write_data(L"			<input name='bModify' type='button' value='卤脿录颅' onclick='window.location.href=modify.html'/>");
	write_data(L"		</p>");
	write_data(L"	</form>");
	write_data(L"");
	write_data(L"</body>");
	write_data(L"</html>");
}

void gError()
{

}



/////////////////////////////////////////////////////////
// args: str=aaaa|bbbb|cccc|   delimiter=|
// return: str = bbbb|cccc|    return value = aaaa
////////////////////////////////////////////////////////
static wchar_t *getStoken(wchar_t **str)
{
    wchar_t *pos,*begin;
    wchar_t delimiter = L'|';

    begin = *str;
    if(!*str)
        return NULL;
    pos = wcschr(*str, delimiter);
    if (!pos) {
      *str=NULL;
      return begin;
  }
  *pos=L'\0';
  if(strlen(*str)>0){
    *str = pos + 1;
}else{
    *str = NULL;
}

return begin;
}

static void ListCurDir(char_t *dir)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    wchar_t DirSpec[MAX_PATH + 1];
    DWORD dwError;

    strncpy (DirSpec, dir, strlen(dir) + 1);
    strncat (DirSpec, L"/*", 3);

    hFind = FindFirstFile(DirSpec, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        DBG(L"Invalid file handle. Error is %u", GetLastError());
        return ;
    }
    else
    {
        if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
        {
        	write_data(L"           <input type=\"checkbox\" name=\"file\" value=\"%s\">%s<br/>\n",FindFileData.cFileName,FindFileData.cFileName);
            //write_data(L"	%s ", FindFileData.cFileName);   
        }
        /*
        else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
            && strcmp(FindFileData.cFileName, ".") != 0
            && strcmp(FindFileData.cFileName, "..") != 0)
        {		
            wchar_t Dir[MAX_PATH + 1];
            strcpy(Dir, pFilePath);
            strncat(Dir, "/", 2);
            strcat(Dir, FindFileData.cFileName);

            FindFile(Dir);
        }
		*/
        while (FindNextFile(hFind, &FindFileData) != 0)
        {
            if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {		
                write_data(L"           <input type=\"checkbox\" name=\"file\" value=\"%s\">%s<br/>\n",FindFileData.cFileName,FindFileData.cFileName);
            }
            /*
            else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
                && strcmp(FindFileData.cFileName, ".") != 0
                && strcmp(FindFileData.cFileName, "..") != 0)
            { 
                wchar_t Dir[MAX_PATH + 1];
                strcpy(Dir, pFilePath);
                strncat(Dir, "/", 2);
                strcat(Dir, FindFileData.cFileName);
                FindFile(Dir);
            }
            */

        }

        dwError = GetLastError();
        FindClose(hFind);
        if (dwError != ERROR_NO_MORE_FILES)
        {
            write_data(L"FindNextFile error. Error is %u ", dwError);
            return;
        }
    }
}

static BOOL DirectoryExists(const char_t* dirName) {
  DWORD attribs = GetFileAttributes(dirName);
  if (attribs == INVALID_FILE_ATTRIBUTES) {
    return false;
}
return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

int _tmain(int argc, _TCHAR* argv[])
{
    char_t opsBuf[10],nameBuf[256],tmpBuf[256];
    HANDLE hXmlFile;
    BOOL ret;
    int nRead = 0,nWritten;

	//must do
    if(init(argc, argv)<0) {
        wprintf(L"[-] cgi file open failed\n");
        exit(-1);
    }
	//create data dir
    if(!DirectoryExists(DATA_DIR)){
      CreateDirectory(DATA_DIR,NULL);
  }

  gogetenv(L"action",opsBuf);
  if(opsBuf){
      if(_wcsicmp(opsBuf,L"home")==0){
       gHome();
   }else if(_wcsicmp(opsBuf,L"list")==0){
       gListFiles();
   }else if(_wcsicmp(opsBuf,L"edit")==0){
       gogetenv(L"file",nameBuf);
       gEditFile(nameBuf);
   }else if(_wcsicmp(opsBuf,L"save")==0){
    gogetenv(L"file",nameBuf);
    gSaveFile(argv[argc-2],nameBuf);
}else if(_wcsicmp(opsBuf,L"new")==0){
    gNewFile();
}else if(_wcsicmp(opsBuf,L"delete")==0){
   gogetenv(L"file",nameBuf);
   gDeleteFile(nameBuf);
}else if(_wcsicmp(opsBuf,L"copy")==0){
   gogetenv(L"file",nameBuf);
   gCopyFile(nameBuf);
}else if(_wcsicmp(opsBuf,L"print")==0){
   gogetenv(L"file",nameBuf);
   gPrintFile(nameBuf);
}else if(_wcsicmp(opsBuf,L"settings")==0){
}else{
   gError();
}
}
destroy();
return 0;
}

	/*
			hXmlFile = CreateFile(buf, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
			//hXmlFile = CreateFile(L"\\data\\app.xml", GENERIC_WRITE, FILE_SHARE_WRITE, NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
			if(hXmlFile != INVALID_HANDLE_VALUE){
				write_data(L"Open sucess");
				do {
					ReadFile(hIn, tmpBuf, 256,(LPDWORD)&nRead,NULL);
					write_data(L"nRead=%d",nRead);
					if(nRead <=0)
						break;
					WriteFile(hXmlFile,tmpBuf,nRead,(LPDWORD)&nWritten,NULL);
				}while(1);
			}else{
				
				write_data(L"Open failed,%d",GetLastError());
			}
			
			CloseHandle(hXmlFile);
			*/