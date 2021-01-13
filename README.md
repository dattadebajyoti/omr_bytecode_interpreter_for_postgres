Using omrjit with postgres:

Before you build make sure these files are placed properly, I have tested the code against postgresql-12.2 version. Following rules best comply with the version
I used.

1. Make the infrastructure for OMR:
   
   Goto contrib directory: "$HOME/project/pgsql/contrib"

    #In the terminal run the following commands:

        cd project/pgsql/contrib/

        mkdir omrjit_expr

        Copy the files: "makefile and omrjit_expr.cpp"  into project/pgsql/contrib/omrjit_expr directory
        
        Now u need to generate the omrjit_expr.so file. For this do the following:
        
        i. Goto the file: 'project/pgsql/src/Makefile.global' in line number 256, and edit "CC=g++"

        ii.In the terminal goto:  project/pgsql/contrib/omrjit_expr,  and run the following commands:

          make

          cp omrjit_expr.so /usr/local/pgsql/lib
        
2. Replace the file "execTuples.c" from "$HOME/project/pgsql/src/backend/executor" with the one in the git.
3. Replace the file "execExprInterp.c" from "$HOME/project/pgsql/src/backend/executor" with the one in the git.
4. Place the file omrjit.h under the dir: "$HOME/project/pgsql/src/include/jit"
5. Replace the file "postgres.c" from "$HOME/project/pgsql/src/backend/tcop" with the one in the git.
6. NOTE: that the CC flag (line: 256) in the Makefile.global at "pgsql/src/Makefile.global" must be gcc while you
   are building postgresql. And when compiling the omrjit_expr.so or any c++ library you need to change the CC flag
   to g++.  

Remember if you building the source code again, make sure to edit "CC=gcc" , but if you need to generate .so file edit it to g++ before generating

