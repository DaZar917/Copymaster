#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "options.h"


void copy( int fd1, int fd2, int bytes){
    char buf1[bytes + 1];
    memset(buf1,0,bytes+1);
    while( ( read  ( fd1 , &buf1 , bytes) )>0 ) {
        write(fd2, &buf1, bytes);
    }
}

int copyError(int fd1, int fd2, int bytes){
    char buf1[bytes + 1];
    memset(buf1,0,bytes+1);
    int error = 0;
    while(  read  ( fd1 , &buf1 , bytes) >0 ) {
        error = write(fd2, &buf1, bytes);
    }
    if (error != -1)
        return 0;
    else return 1;
}

void create(int fd1, int fd2, mode_t rules,const char* filename){
    chmod(filename, rules);
    copy(fd1,fd2,1);
}

void links(const char* filename1, const char* filename2){
    link(filename1,filename2);
}


int fileSize(const char* filename, off_t size){
    return truncate(filename, size);
}

int copyFromPos(int fd1, int fd2, long x, int pos1, int pos2, int num){
    char buf[num + 1];
    memset(buf,0,num+1);
    int error = 0;

    error = lseek(fd1, pos1, SEEK_SET);
    if (error == -1) {
        return 1;           //TODO    lseek infile fail
    }

    error = read  ( fd1 , &buf , num);
    if (error == -1) {
        return 2;            //TODO    read/write fail
    }

    error = lseek(fd2, pos2, x);
    if (error == -1) {
        return 3;           //TODO    lseek outfile fail
    }

    error = write(fd2,&buf,num);
    if (error == -1) {
        return 2;           //TODO    read/write fail
    }
    return 0;
}

int node(const char* filename1,int fd1, int fd2,  ino_t inode ){
    struct stat fileStat;
    stat(filename1,&fileStat);
    int error = copyError(fd1,fd2,1);

    if( inode == fileStat.st_ino ){
        copyError(fd1,fd2,1);
    } else if ( inode != fileStat.st_ino ) {
        return 1;                     //TODO    inode fail
    }else if (S_IFREG == fileStat.st_mode){
        return 2;                     //TODO    regular fail
    } else if (error != 0){
        return 3;                     //TODO    another error
    }
    return 0;
}

bool allCommandsIsOff(struct CopymasterOptions cpm_options){
    if (cpm_options.fast == 0 && cpm_options.slow == 0 && cpm_options.create == 0 && cpm_options.overwrite == 0 && cpm_options.append == 0 && cpm_options.lseek == 0 && cpm_options.directory == 0 && cpm_options.delete_opt == 0 && cpm_options.chmod == 0 && cpm_options.inode == 0 && cpm_options.umask == 0 && cpm_options.link == 0 && cpm_options.truncate == 0 && cpm_options.sparse == 0){
        return true;
    } else return false;
}

bool errorPrep(struct CopymasterOptions cpm_options){
    if ( cpm_options.append == 1 && cpm_options.create == 1 ) return true;
    else if ( cpm_options.append == 1 && cpm_options.overwrite == 1 ) return true;
    else if ( cpm_options.append == 1 && cpm_options.lseek == 1 ) return true;
    else if ( cpm_options.append == 1 && cpm_options.umask == 1 ) return true;
    else if ( cpm_options.append == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.create == 1 && cpm_options.overwrite == 1 ) return true;
    else if ( cpm_options.create == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.overwrite == 1 && cpm_options.lseek == 1 ) return true;
    else if ( cpm_options.overwrite == 1 && cpm_options.chmod == 1 ) return true;
    else if ( cpm_options.overwrite == 1 && cpm_options.umask == 1 ) return true;
    else if ( cpm_options.overwrite == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.fast == 1 && cpm_options.slow == 1 ) return true;
    else if ( cpm_options.fast == 1 && cpm_options.directory == 1 ) return true;
    else if ( cpm_options.fast == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.fast == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.lseek == 1 && cpm_options.chmod == 1 ) return true;
    else if ( cpm_options.lseek == 1 && cpm_options.umask == 1 ) return true;
    else if ( cpm_options.lseek == 1 && cpm_options.directory == 1 ) return true;
    else if ( cpm_options.lseek == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.lseek == 1 && cpm_options.sparse == 1 ) return true;
    else if ( cpm_options.delete_opt == 1 && cpm_options.directory == 1 ) return true;
    else if ( cpm_options.delete_opt == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.delete_opt == 1 && cpm_options.truncate == 1 ) return true;
    else if ( cpm_options.slow == 1 && cpm_options.directory == 1 ) return true;
    else if ( cpm_options.slow == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.chmod == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.umask == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.inode == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.directory == 1 && cpm_options.link == 1 ) return true;
    else if ( cpm_options.directory == 1 && cpm_options.truncate == 1 ) return true;
    else if ( cpm_options.directory == 1 && cpm_options.sparse == 1 ) return true;
    else if ( cpm_options.link == 1 && cpm_options.sparse == 1 ) return true;

    else return false;
}
































struct CopymasterOptions ParseCopymasterOptions(int argc, char *argv[])
{
    extern int optind;
    extern char* optarg;







    struct CopymasterOptions cpm_options = {0};




    char c = 0, x = 0, *tok = 0;
    unsigned long long num = 0;
    long pos1 = -1, pos2 = -1;
    int i = 0;



    const char* usage_error_msg_format = "Usage: %s [OPTION]... SOURCE DEST\n(%s)\n";

    if (argc <= 0) {
        fprintf(stderr, usage_error_msg_format, "copymaster", "argc <= 0");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int option_index = 0;

        static struct option long_options[] = {
                { "fast",      no_argument,       0, 'f' },
                { "slow",      no_argument,       0, 's' },
                { "create",    required_argument, 0, 'c' },
                { "overwrite", no_argument,       0, 'o' },
                { "append",    no_argument,       0, 'a' },
                { "lseek",     required_argument, 0, 'l' },
                { "directory", required_argument, 0, 'D' },
                { "delete",    no_argument,       0, 'd' },
                { "chmod",     required_argument, 0, 'm' },
                { "inode",     required_argument, 0, 'i' },
                { "umask",     required_argument, 0, 'u' },
                { "link",      no_argument,       0, 'K' },
                { "truncate",  required_argument, 0, 't' },
                { "sparse",    no_argument,       0, 'S' },
                { 0,             0,               0,  0  },
        };
        c = getopt_long(argc, argv, "fsc:oal:Ddm:i:u:Kt:S",
                        long_options, &option_index);
        if (c == -1)
            break;









        switch (c)
        {
//-------------------------------------------------fast-------------------------------------------------//
            case 'f':
                cpm_options.fast = 1;
                break;
//-------------------------------------------------slow-------------------------------------------------//
            case 's':
                cpm_options.slow = 1;
                break;
//-------------------------------------------------CREAT-------------------------------------------------//
            case 'c':
                cpm_options.create = 1;
                sscanf(optarg, "%ho", &cpm_options.create_mode);
                break;









//-----------------------------------------------OVERWRITE-----------------------------------------------//
            case 'o':
                cpm_options.overwrite = 1;
                break;
//-----------------------------------------------OVETWRITE-----------------------------------------------//



//------------------------------------------------APPEND-------------------------------------------------//
            case 'a':
                cpm_options.append = 1;

                break;
//------------------------------------------------APPEND-------------------------------------------------//



//-------------------------------------------------LSEEK-------------------------------------------------//
            case 'l':
                cpm_options.lseek = 1;
                i = sscanf(optarg, "%c,%ld,%ld,%llu", &x, &pos1, &pos2, &num);
                if (i < 4) {
                    fprintf(stderr, usage_error_msg_format, argv[0], "lseek - failed to scan 4 values");
                    exit(EXIT_FAILURE);
                }
                cpm_options.lseek_options.pos1 = (off_t)pos1;
                cpm_options.lseek_options.pos2 = (off_t)pos2;
                cpm_options.lseek_options.num = (size_t)num;
                switch (x)
                {
                    case 'b':
                        cpm_options.lseek_options.x = SEEK_SET;
                        break;
                    case 'e':
                        cpm_options.lseek_options.x = SEEK_END;
                        break;
                    case 'c':
                        cpm_options.lseek_options.x = SEEK_CUR;
                        break;
                    default:
                        fprintf(stderr, usage_error_msg_format, argv[0], "lseek - invalid value of x");
                        exit(EXIT_FAILURE);
                        break;
                }
                break;


//--------------------------------------------------LSEEK------------------------------------------------//



//-------------------------------------------------DIRECT------------------------------------------------//
            case 'D':
                cpm_options.directory = 1;
                break;
//-------------------------------------------------DIRECT------------------------------------------------//



//-------------------------------------------------DELETE------------------------------------------------//
            case 'd':
                cpm_options.delete_opt = 1;
                break;
//-------------------------------------------------DELETE------------------------------------------------//



//-------------------------------------------------CHMOD-------------------------------------------------//
            case 'm':
                cpm_options.chmod = 1;
                sscanf(optarg, "%ho", &cpm_options.chmod_mode);
                break;
//-------------------------------------------------CHMOD-------------------------------------------------//


//-------------------------------------------------INODE-------------------------------------------------//
            case 'i':
                cpm_options.inode = 1;
                sscanf(optarg, "%llu", &cpm_options.inode_number);
                break;
//-------------------------------------------------INODE-------------------------------------------------//



//-------------------------------------------------UMASK-------------------------------------------------//
            case 'u':
                cpm_options.umask = 1;
                tok = strtok(optarg, ",");
                i = 0;
                while (tok != NULL && i < 9) {
                    if (strlen(tok) != 3) {
                        fprintf(stderr, usage_error_msg_format, argv[0], "umask - unexpected value of UTR option");
                        exit(EXIT_FAILURE);
                    }
                    strncpy(cpm_options.umask_options[i], tok, 3);
                    tok = strtok(NULL, ",");
                    ++i;
                }
                memset(cpm_options.umask_options[i], 0, 4);
                break;
//-------------------------------------------------UMASK-------------------------------------------------//



//-------------------------------------------------LINK-------------------------------------------------//
            case 'K':
                cpm_options.link = 1;
                break;
//-------------------------------------------------Link------------------------------------------------//



//-------------------------------------------------TRUNCKATE-------------------------------------------------//
            case 't':
                cpm_options.truncate = 1;
                sscanf(optarg, "%ld", &pos1);
                cpm_options.truncate_size = (off_t)pos1;

                break;
                //-------------------------------------------------TRUNCKATE-------------------------------------------------//

            case 'S':
                cpm_options.sparse = 1;
                break;

            case '?':
                default:
                    fprintf(stderr, usage_error_msg_format, argv[0]);
                    exit(EXIT_FAILURE);
        }
    }

    if (optind + 2 != argc) {
        fprintf(stderr, usage_error_msg_format, argv[0], "infile or outfile is missing");
        exit(EXIT_FAILURE);
    }

    cpm_options.infile = argv[optind];
    cpm_options.outfile = argv[optind + 1];

    return cpm_options;
}

