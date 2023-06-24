clear
gcc ./src/master.c -lm -lrt  -o ./bin/master
gcc ./src/processA_normal.c -lbmp -lncurses -lm -lrt -o ./bin/processA_normal
gcc ./src/processA_server.c -lbmp -lncurses -lm -lrt -o ./bin/processA_server
gcc ./src/processA_client.c -lbmp -lncurses -lm -lrt -o ./bin/processA_client
gcc ./src/processB.c -lbmp -lncurses -lm -lrt -o ./bin/processB
./bin/master
