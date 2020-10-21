Using omrjit with postgres:

Before you build make sure these files are placed properly, I have tested the code against postgresql-12.2 version. Following rules best comply with the version
I used.

1. Place the "omrjit_expr" dir under the contrib directory: "../pgsql/contrib"
2. Replace the file "execTuples.c" from "pgsql/src/backend/executor" with the one in the git.
3. Place the file omrjit.h under the dir: "pgsql/src/include/jit"
4. Replace the file "postgres.c" from "pgsql/src/backend/tcop" with the one in the git.
5. NOTE: that the CC flag (line: 256) in the Makefile.global at "pgsql/src/Makefile.global" must be gcc while you
   are building postgresql. And when compiling the omrjit_expr.so or any c++ library you need to change the CC flag
   to g++.
6. Currently, I haven't properly pushed the code, in that case I would require to meet all the testcases of postgresql.
7. Refer, the postgres_installation document for more into installing, building with eclipse, creating a 10Million row table and testing your queries.
