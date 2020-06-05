#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <zconf.h>
#include <stdbool.h>

#include "options.h"




void FatalError(char c, const char* msg, int exit_status);
void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);

int main(int argc, char* argv[])
{



//TODO------------------------------------bez prepinacov--------------------------------------//

    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);

    int fileDes1,fileDes2=0;

    fileDes1 = open(cpm_options.infile, O_RDONLY);
    fileDes2 = open(cpm_options.outfile, O_RDONLY);

    struct stat fdStat, fdStat2;

    lstat( argv[1] , &fdStat );
    lstat( argv[2] , &fdStat2 );






    if ( fileDes1 == -1 && allCommandsIsOff(cpm_options) == true) {        //TODO перевірка чи існує INFILE
        perror("SUBOR NEEXISTUJE");
        exit(21);
    }


    if (fdStat2.st_mode == 32768) {             //TODO перевірка чи OUTFILE має права на запис
        perror("INA CHYBA");
        exit(21);
    }











//TODO------------------------------------bez prepinacov--------------------------------------//

    if ( fileDes2 >= 0 && allCommandsIsOff( cpm_options ) == true ){
        fileDes2 = open(cpm_options.outfile, O_CREAT|O_TRUNC|O_WRONLY);
        copy(fileDes1,fileDes2,1);
    }
    else if ( fileDes2 == -1 && allCommandsIsOff(cpm_options) == true){
        fileDes2 = open(cpm_options.outfile, O_CREAT| O_TRUNC |O_WRONLY);
        chmod(cpm_options.outfile,fdStat.st_mode);
        copy(fileDes1,fileDes2,1);
    }

//TODO------------------------------------bez prepinacov--------------------------------------//


if (allCommandsIsOff(cpm_options) == false){
    bool error = errorPrep(cpm_options);
    if (error == true){
        perror("CHYBA PREPINACOV");
        exit(20);
    }
}



//TODO------------------------------------create--------------------------------------//
    if(cpm_options.create == 1) {
        if ((fileDes2 = open(cpm_options.outfile, O_RDONLY)) >= 0) {                      //TODO перевірка чи існує INFILE для препінача create
            perror("SUBOR EXISTUJE");
            exit(23);
        } else if (open(cpm_options.outfile, O_RDONLY) == -1 ){                           //TODO перевірка чи існує INFILE для препінача create
            fileDes2 = open(cpm_options.outfile, O_CREAT|O_WRONLY);
            create(fileDes1,fileDes2,cpm_options.create_mode,cpm_options.outfile);

        }
    }
//TODO------------------------------------create--------------------------------------//


//TODO------------------------------------fast--------------------------------------//
    if(cpm_options.fast == 1 ){
        fileDes1 = open(cpm_options.infile, O_RDONLY);
        fileDes2 = open(cpm_options.outfile, O_CREAT|O_WRONLY);
        int error = copyError(fileDes1,fileDes2,40000);
        copy(fileDes1,fileDes2,40000);
        if (error != 0)
            perror("FAST");
    }
//TODO------------------------------------fast--------------------------------------//


//TODO------------------------------------slow--------------------------------------//
    if ( cpm_options.slow == 1 ){
        fileDes1 = open(cpm_options.infile, O_RDONLY);
        fileDes2 = open(cpm_options.outfile, O_CREAT|O_TRUNC|O_WRONLY);
        int error = copyError(fileDes1,fileDes2,1);
        copy(fileDes1,fileDes2,1);
        if (error != 0)
            perror("OPENSLOW");

    }
//TODO------------------------------------slow--------------------------------------//



//TODO------------------------------------link--------------------------------------//
if (cpm_options.link == 1) {
    if (open(cpm_options.infile, O_RDONLY) >= 0) {
        links(cpm_options.infile, cpm_options.outfile);
        if (open(cpm_options.outfile, O_RDONLY) == -1) {
            perror("VYSTUPNY SUBOR NEVYTVORENY");
            exit(30);
        }
    }else {
        perror("VSTUPNY SUBOR NEEXISTUJE");
        exit(30);
    }


}
//TODO------------------------------------link--------------------------------------//


//TODO------------------------------------trunc--------------------------------------//

if(cpm_options.truncate == 1){
    fileDes1 = open(cpm_options.infile, O_RDONLY);
    fileDes2 = open(cpm_options.outfile, O_WRONLY);

    int error = copyError(fileDes1,fileDes2,1);

    if (error == 0) {
        copy(fileDes1, fileDes2, 1);
        if (fileSize(cpm_options.infile, cpm_options.truncate_size) != -1 ) {
            fileSize(cpm_options.infile, cpm_options.truncate_size);
        } else if (fileSize(cpm_options.infile, cpm_options.truncate_size) == -1 ) {
            perror("VSTUPNY SUBOR NEZMENENY");
            exit(31);
        }
    }else {
        perror("INA CHYBA");
        exit(31);
    }

}
//TODO------------------------------------trunc--------------------------------------//



//TODO------------------------------------lseek--------------------------------------//



if (cpm_options.lseek == 1 ) {
    fileDes2 = open(cpm_options.outfile, O_CREAT | O_RDWR);
    int error;
    if ((error = copyFromPos(fileDes1, fileDes2, cpm_options.lseek_options.x, cpm_options.lseek_options.pos1,
                             cpm_options.lseek_options.pos2, cpm_options.lseek_options.num)) == 0) {
        copyFromPos(fileDes1, fileDes2, cpm_options.lseek_options.x, cpm_options.lseek_options.pos1,
                    cpm_options.lseek_options.pos2, cpm_options.lseek_options.num);
    } else if (error == 1) {
        perror("CHYBA POZICIE infile");
        exit(33);
    } else if (error == 2){
        perror("INA CHYBA");
        exit(33);
    }else if (error == 3) {
        perror("CHYBA POZICIE outfile");
        exit(33);
    }
}
//TODO------------------------------------lseek--------------------------------------//


//TODO------------------------------------inode--------------------------------------//

if (cpm_options.inode == 1){
    int error = 0;
    fileDes2 = open(cpm_options.outfile, O_RDONLY);

    error = node(cpm_options.infile, fileDes1, fileDes2, cpm_options.inode_number) ;

    if (error != 0) {
        if (error == 1) {
            perror("ZLY INODE");
            exit(27);
        } else if (error == 2) {
            perror("ZLY TYP VSTUPNEHO SUBORU");
            exit(27);
        } else if (error == 3) {
            perror("INA CHYBA");
            exit(27);
        }
    } else {
        fileDes2= open(cpm_options.outfile, O_CREAT|O_RDWR);
        node(cpm_options.infile,fileDes1,fileDes2,cpm_options.inode_number);
    }

}

//TODO------------------------------------inode--------------------------------------//


//TODO------------------------------------chmod--------------------------------------//
if (cpm_options.chmod == 1){
    fileDes2 = open(cpm_options.outfile, O_CREAT|O_WRONLY);
    perror("CHMOD");
    copy(fileDes1,fileDes2,1);
    chmod(cpm_options.outfile, cpm_options.chmod_mode);
}
//TODO------------------------------------chmod--------------------------------------//


//TODO------------------------------------delete--------------------------------------//


if (cpm_options.delete_opt == 1){
    fileDes1 = open(cpm_options.infile, O_RDONLY);
    if (fileDes1 == -1){
        perror("SUBOR NEEXISTUJE");
        exit(21);
    }
    fileDes2 = open(cpm_options.outfile, O_RDWR);

    int error = copyError(fileDes1,fileDes2,1);
    struct stat fileStat;

    stat(cpm_options.infile,&fileStat);
    if ( error == 0 ){
        copyError(fileDes1,fileDes2,1);
        unlink(cpm_options.infile);
    } else{
        perror("SUBOR NEBOL ZMAZANY");
        exit(26);
    }

}
//TODO------------------------------------delete--------------------------------------//


//TODO------------------------------------append--------------------------------------//
if (cpm_options.append == 1){
    fileDes1 = open(cpm_options.infile, O_RDONLY);
    fileDes2 = open(cpm_options.outfile, O_APPEND | O_WRONLY );
    int error =  copyError(fileDes1,fileDes2,1);
    copyError(fileDes1,fileDes2,1);
    if ( fileDes2 == -1){
        perror("SUBOR NEEXISTUJE");
        exit(22);
    } else if (error != 0){
        perror("INA CHYBA");
        exit(22);
    }
}
//TODO------------------------------------append--------------------------------------//


//TODO------------------------------------overwrite--------------------------------------//

if (cpm_options.overwrite == 1){
    fileDes1 = open(cpm_options.infile, O_RDONLY);
    fileDes2 = open(cpm_options.outfile, O_TRUNC | O_WRONLY );
    int error = copyError(fileDes1,fileDes2,1);
    if (fileDes2 == -1){
        perror("SUBOR NEEXISTUJE");
        exit(24);
    } else if (error != 0){
        perror("INA CHYBA");
        exit(24);
    } else copy(fileDes1,fileDes2,1);
}

//TODO------------------------------------overwrite--------------------------------------//





















    PrintCopymasterOptions(&cpm_options);
    if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n");
        exit(EXIT_FAILURE);
    }

    if (cpm_options.directory) {

    }

    return 0;
}


void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno);
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}

void PrintCopymasterOptions(struct CopymasterOptions* cpm_options)
{
    if (cpm_options == 0)
        return;

    printf("infile:        %s\n", cpm_options->infile);
    printf("outfile:       %s\n", cpm_options->outfile);

    printf("fast:          %d\n", cpm_options->fast);
    printf("slow:          %d\n", cpm_options->slow);
    printf("create:        %d\n", cpm_options->create);
    printf("create_mode:   %o\n", (unsigned int)cpm_options->create_mode);
    printf("overwrite:     %d\n", cpm_options->overwrite);
    printf("append:        %d\n", cpm_options->append);
    printf("lseek:         %d\n", cpm_options->lseek);

    printf("lseek_options.x:    %d\n", cpm_options->lseek_options.x);
    printf("lseek_options.pos1: %ld\n", cpm_options->lseek_options.pos1);
    printf("lseek_options.pos2: %ld\n", cpm_options->lseek_options.pos2);
    printf("lseek_options.num:  %lu\n", cpm_options->lseek_options.num);

    printf("directory:     %d\n", cpm_options->directory);
    printf("delete_opt:    %d\n", cpm_options->delete_opt);
    printf("chmod:         %d\n", cpm_options->chmod);
    printf("chmod_mode:    %o\n", (unsigned int)cpm_options->chmod_mode);
    printf("inode:         %d\n", cpm_options->inode);
    printf("inode_number:  %lu\n", cpm_options->inode_number);

    printf("umask:\t%d\n", cpm_options->umask);
    for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options->umask_options[i][0] == 0) {

            break;
        }
        printf("umask_options[%u]: %s\n", i, cpm_options->umask_options[i]);
    }

    printf("link:          %d\n", cpm_options->link);
    printf("truncate:      %d\n", cpm_options->truncate);
    printf("truncate_size: %ld\n", cpm_options->truncate_size);
    printf("sparse:        %d\n", cpm_options->sparse);
}
