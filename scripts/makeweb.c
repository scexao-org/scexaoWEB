#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>


int MAKEALL = 0; // set to 1 if all directories need to be built, otherwise, only the directories for which the .txt file has been updated will be (re)created



int WRITE_NUMBER_SUBDIR = 0;

typedef struct
{
    char dirname[50][50]; // levels 0..
    char fulldir[500];
    int active;   // 1 if webpage created, otherwise, menu entry only 
    int  update; // 0 if no update, 1 if update
    char title[500];
    long index; // first 3 digits
    long level;
    long parent; // index of parent page
    long nbchild; // number of children
    long nbchild1; // number of children + grandchildren + ...
} WEBDIR;



void remove_char_from_string(char c, char *str)
{
    int i=0;
    int len = strlen(str)+1;

    for(i=0; i<len; i++)
    {
        if(str[i] == c)
        {
            // Move all the char following the char "c" by one to the left.
            strncpy(&str[i],&str[i+1],len-i);
        }
    }
}


int main(int argc, char **argv)
{
    FILE *fp;
    FILE *fp1;
    char *token;
    char fname[600];
    char fnamemd[200];
    WEBDIR webdir[2000];
    char line[200];
    char str0[200];
    char str1[200];
    char strchild[200];
    char *sep = "./";
    long webdirnb = 0;
    long webdirnb1;
    char command[2000];
    long level,level1;
    long i,l;
    long wd,wd1;
    char fulldirectory[500];
    char titleline[200];
    int OK;
    long lastlevel;
    int hfont;
    struct stat filestat_in_html;
    struct stat filestat_out_html;
    struct stat filestat_txt;
    struct stat filestat_introtxt;
    struct stat filestat_titletxt;
    char updatetime[200];
    char updatetimetxt[200];
    double tinterval;
    double tintervaltitle;

	char indexstr[4];
	
	
	
    int mdmode = 0; // 1 if .md file is input to webpage -> then, use md2html

	
	
    if(argc==2)
    {
        if(strcmp(argv[1], "all")==0)
        {
            MAKEALL = 1;
        }
    }
	



    // establish list of .web directories

    printf("Scanning directories ...\n");
    fflush(stdout);
    system("rm listdir.txt &> /dev/null");
    system("find . -type d -name \"*.web\" | sort > listdir.txt");


	updatetime[0] = '\0';
	updatetimetxt[0] = '\0';



    fp = fopen("listdir.txt", "r");
    while ((fscanf(fp,"%s\n", line))!=EOF)
    {
        webdir[webdirnb].active = 1; // by default, create a page
        level = 0;
        strcpy(webdir[webdirnb].fulldir, line);
        token = strtok(line, sep);
        strcpy(webdir[webdirnb].dirname[level], token);
        while((token=strtok(NULL,sep))!=NULL)
        {
            if(strcmp(token, "web")!=0)
            {
                level++;
                strcpy(webdir[webdirnb].dirname[level], token);
            }
        }
        webdir[webdirnb].level = level;



        webdir[webdirnb].update = 0;


        // CHECK THAT FILES EXIST: title.txt (mandatory) and content.html (optional)

        sprintf(fname, "%s/title.txt", webdir[webdirnb].fulldir);
        if (!stat(fname, &filestat_titletxt)) {
            if((fp1 = fopen(fname, "r"))!=NULL) {
				printf("--- READING file %s\n", fname);
                fgets(titleline, 200, fp1);
                fclose(fp1);
                strcpy(webdir[webdirnb].title, titleline);
            }
            else
            {
				printf("ERROR: Cannot read file %s\n", fname);
				exit(0);
			}
			remove_char_from_string('\n', webdir[webdirnb].title);
        } else {
            printf("\033[31;1m ERROR [%d]: File %s: does not exist.\033[0m\n", __LINE__, fname);
            exit(0);
        }



        // INPUT  : content.html
        // OUTPUT : indexm.html

        sprintf(fname,"%s/content.html",webdir[webdirnb].fulldir);
        if (!stat(fname, &filestat_in_html)) {
            //strftime(updatetime, 100, "%d/%m/%Y %H:%M:%S", localtime( &filestat_html.st_mtime));
            //	printf("File %s last modified date and time = %s\n", fname, t);
        } else {
            //printf("WARNING [%d]: File %s: does not exist.\n", __LINE__, fname);
            webdir[webdirnb].active = 0;
        }

        if(webdir[webdirnb].active == 1)
        {
            sprintf(fname,"%s/indexm.html",webdir[webdirnb].fulldir);
            if (!stat(fname, &filestat_out_html)) {
            } else {
            }
        }


        tinterval = difftime(filestat_out_html.st_mtime, filestat_in_html.st_mtime);
        tintervaltitle = difftime(filestat_out_html.st_mtime, filestat_titletxt.st_mtime);

        if((tinterval < 0.0)||(tintervaltitle < 0.0))
        {
            webdir[webdirnb].update = 1;
        }
		webdir[webdirnb].update = 1;
        
        webdirnb++;
    }
    fclose(fp);




    printf("\n\n");
    for(wd=0; wd<webdirnb; wd++)
    {
		indexstr[0] = webdir[wd].dirname[webdir[wd].level][0];
		indexstr[1] = webdir[wd].dirname[webdir[wd].level][1];
		indexstr[2] = webdir[wd].dirname[webdir[wd].level][2];
		indexstr[3] = '\0';
		
		webdir[wd].index = atoi(indexstr);
		
        if((webdir[wd].update == 1)&&(webdir[wd].active == 1))
            printf("\033[32;1m");

        printf("[%3ld  %04ld] [%50s] %d %d %-50s (level = %2ld): ",wd, webdir[wd].index, webdir[wd].title, webdir[wd].active, webdir[wd].update, webdir[wd].fulldir, webdir[wd].level);
        for(l=0; l<webdir[wd].level+1; l++)
            printf("[%2ld %2ld](%-16s) ", wd, l, webdir[wd].dirname[l]);

        printf("\033[0m");
        printf("\n");
    }
    printf("\n\n");



    printf("Count number of children\n");
    fflush(stdout);


    for(wd=0; wd<webdirnb; wd++)
    {
        webdir[wd].nbchild = 0;
        level = webdir[wd].level;
        for(wd1=0; wd1<webdirnb; wd1++)
        {
            level1 = webdir[wd1].level;
            if(level1==level+1)
                if(strcmp(webdir[wd1].dirname[level],webdir[wd].dirname[level])==0)
                    webdir[wd].nbchild++;
            if(level1>level)
                if(strcmp(webdir[wd1].dirname[level],webdir[wd].dirname[level])==0)
                    webdir[wd].nbchild1++;
        }
    }

    printf("\n");








    // make index.html
    if(1==0)
    {
        printf("Writing index.html ...\n");

        lastlevel = 0;
        fp = fopen("webpage_sec2.txt", "w"); // this is the menu

        fprintf(fp,"<h3>Home</h3><br><hr>");

        for(wd=0; wd<webdirnb; wd++)
        {
            if(webdir[wd].level<lastlevel+1)
            {
                //if(webdir[wd].level==0)
                //    fprintf(fp, "");

                str0[0] = '\0';
                for(i=0; i<webdir[wd].level; i++)
                    strcat(str0, "-");

                strchild[0] = '\0';
                if(WRITE_NUMBER_SUBDIR==1)
                    if((webdir[wd].level==lastlevel) && (webdir[wd].nbchild>0))
                    {
                        if(webdir[wd].nbchild==webdir[wd].nbchild1)
                            sprintf(strchild," (%ld)", webdir[wd].nbchild);
                        else
                            sprintf(strchild," (%ld,%ld)", webdir[wd].nbchild, webdir[wd].nbchild1);
                    }

                //	  if(webdir[wd].level==0)
                if(strlen(webdir[wd].title)>1)
                    fprintf(fp,"<div class=\"menul%ld\">%s<a href=\"%s/content.html\" style=\"text-decoration: none\"> <font color=\"00000\"> %s</font> </a>%s</div>\n",webdir[wd].level, str0, webdir[wd].fulldir, webdir[wd].title, strchild);
                //else
                // fprintf(fp,"%s<a href=\"%s/%s.html\">%s</a>%s<br>\n",str0,webdir[wd].fulldir,webdir[wd].dirname[0],webdir[wd].title,strchild);
            }
        }
        fclose(fp);
        sprintf(command, "./scripts/fileupdatetime.txt index.txt > updatetime.txt"); //, webdir[wd].fulldir, webdir[wd].dirname[webdir[wd].level]);
        system(command);

        system("cat webpage_sec1.0.txt index.intro.txt webpage_sec1.1.txt webpage_sec2.txt webpage_sec3.txt index.txt webpage_sec5.txt updatetime.txt webpage_sec6.txt > index_auto.html");

        system("cp webpic_home.png webpic.png");
        system("./scripts/mkwebpicgrad.txt");
        system("mv webpic_gd.jpg webpic_home.jpg");
        printf("\n");
    }




    // MAKE SITE MAP

    // site map is always recreated
    printf("Making site map ...\n");
    fp = fopen("sitemap.txt", "w");
    for(wd=0; wd<webdirnb; wd++)
        if(strlen(webdir[wd].title)>1) {
            hfont = webdir[wd].level+2;
            if(hfont > 4)
                hfont = 4;
            hfont = 4;
            str0[0] = '\0';
            str1[0] = '\0';

            if(webdir[wd].level==0)
                str0[0] = '\0';


            for(i=0; i<webdir[wd].level; i++)
                strcat(str0,"&nbsp;&nbsp;&nbsp;&nbsp;");
            if(webdir[wd].level==0)
                fprintf(fp,"%s<b><a href=\"../%s/content.html\" style=\"text-decoration: none\"><font color=\"00000\"> %s </font> </a></b><br>%s\n",str0,webdir[wd].fulldir,webdir[wd].title,str1);
            else
                fprintf(fp,"%s<a href=\"../%s/content.html\" style=\"text-decoration: none\"><font color=\"00000\"> %s </font> </a><br>%s\n",str0,webdir[wd].fulldir,webdir[wd].title,str1);
        }
    fclose(fp);
    //  system("mv sitemap.txt ./99sitemap.web/content.txt");
    printf("\n");






    // make every page
    printf("Making web pages ...\n");
    for(wd=0; wd<webdirnb; wd++)
        if(webdir[wd].update == 1)
        {
			char fnamemenu[600];
			char fname1[600];
			
			int toggleindex100 = 0;
			int toggleindex200 = 0;
			
            printf("Working on directory %s ...\n",webdir[wd].fulldir);
            // make webpage_sec2.txt
            level = webdir[wd].level+1;
            //    lastlevel = webdir[wd].level+1;

            
            sprintf(fnamemenu, "%s/page_menu.txt", webdir[wd].fulldir);
            printf("   Creating File %s\n", fnamemenu);
            fp = fopen(fnamemenu, "w"); // this is the menu

            str1[0] = '\0';
            for(i=0; i<webdir[wd].level+1; i++)
                strcat(str1, "../");


			fprintf(fp, "</div>\n");
			
			
            fprintf(fp, "<div id='cssmenu'>\n");
            fprintf(fp, "<ul>\n");


            for(wd1=0; wd1<webdirnb; wd1++)
            {
               // if(webdir[wd1].level == 0)
               //     fprintf(fp,"");

            
            if(webdir[wd1].level==0)
            {
				if(webdir[wd1].index > 99)
					toggleindex100++;

				if(webdir[wd1].index > 199)
					toggleindex200++;
			}

				if(toggleindex100==1) // first time we have index 100 or more
				{
					fprintf(fp, "<!--index100-->\n");
					toggleindex100 ++;
				}
				if(toggleindex200==1) // first time we have index 200 or more
				{
					fprintf(fp, "<!--index200-->\n");
					toggleindex200 ++;
				}

                str0[0] = '\0';
                //  for(i=0;i<webdir[wd1].level;i++)
                // strcat(str0,"&nbsp;&nbsp;&nbsp;");

                strchild[0] = '\0';
                if(WRITE_NUMBER_SUBDIR==1)
                    if(webdir[wd1].nbchild>0)
                    {
                        if(webdir[wd1].nbchild==webdir[wd1].nbchild1)
                            sprintf(strchild," (%ld)",webdir[wd1].nbchild);
                        else
                            sprintf(strchild," (%ld,%ld)",webdir[wd1].nbchild,webdir[wd1].nbchild1);
                    }

                level1 = webdir[wd1].level;
                for(i=0; i<level; i++) // scan all levels above current one
                    if(strcmp(webdir[wd1].dirname[level1],webdir[wd].dirname[i])==0)
                        strchild[0] = '\0';


                if(wd==wd1)
                {
                    //	      if(webdir[wd1].level == 0)
                    fprintf(fp,"<div class=\"active menul%ld\">%s<b>%s</b></div>\n", webdir[wd1].level, str0, webdir[wd1].title);
                    //else
                    //	fprintf(fp,"%s<b>%s</b><br>\n",str0,webdir[wd1].title);
                }
                else
                {
                    OK = 0;
                    if(webdir[wd1].level == 0)
                        OK = 1;
                    else
                    {
                        if(webdir[wd].level>0)
                            if(strcmp(webdir[wd1].dirname[webdir[wd1].level-1],webdir[wd].dirname[webdir[wd].level-1])==0)
                                OK = 1; // same parent
                    }

                    for(i=0; i<level; i++) // scan all levels above current one
                        if(strcmp(webdir[wd1].dirname[level1],webdir[wd].dirname[i])==0)
                            OK = 1; // parent

                    if(webdir[wd1].level>0)
                    {
                        if(strcmp(webdir[wd1].dirname[webdir[wd1].level-1],webdir[wd].dirname[webdir[wd].level])==0)
                            OK = 1; // level below
                    }

                    if(OK==1)
                    {
                        //		  if(webdir[wd1].level == 0)
                        fprintf(fp,"<div class=\"menul%ld\">",webdir[wd1].level);
                        //else
                        // fprintf(fp,"<b>");

                        str1[0] = '\0';
                        for(i=0; i<webdir[wd].level+1; i++)
                            strcat(str1,"../");
                        
                        char titlestring[600];
                        if(webdir[wd1].nbchild>0)
							sprintf(titlestring, "%s >", webdir[wd1].title);
						else
							sprintf(titlestring, "%s", webdir[wd1].title);
                        

							fprintf(fp,"%s<a href=\"%s%s/indexm.html\" style=\"text-decoration: none\"> <font color=\"00000\"> %s </font> </a>%s", 
							str0, 
							str1,
							webdir[wd1].fulldir,
							titlestring,
							strchild
							);
						
                        
                        
                        //		  if(webdir[wd1].level == 0)
                        fprintf(fp,"</div>\n");
                        //else
                        //fprintf(fp,"</b><br>\n");
                    }
                }
            }
            fprintf(fp, "</ul>\n");
            fprintf(fp, "</div>\n\n");
            fclose(fp);
            
            
            if(webdir[wd].active == 1)
            {	
				sprintf(command, "./mkpage %s \"%s\" %ld", webdir[wd].fulldir, webdir[wd].title, webdir[wd].level);
				system(command);
			}
            



                sprintf(fname,"%s/content.html",webdir[wd].fulldir);
                if (!stat(fname, &filestat_in_html)) {
                    strftime(updatetime, 200, "%d/%m/%Y %H:%M:%S %Z", localtime( &filestat_in_html.st_mtime));
                    printf("File %s last modified date and time = %s\n", fname, updatetime);
                }
                




					sprintf(fname1, "%s/page_modtime.txt", webdir[wd].fulldir);
                    fp = fopen(fname1,"w");
                    fprintf(fp, "<hr>\n");
                    fprintf(fp,"File %s last modified  %s<br>", fname, updatetime);
                    fclose(fp);

        }


    return(0);
}

