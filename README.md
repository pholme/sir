# Fast SIR on networks
Pretty quick code for regular (continuous time, Markovian) SIR on simple graphs. I chatty intro to it here: https://petterhol.me/2018/02/07/fastest-network-sir-code-in-the-east/

To get it running you need:

1. Make dictionary o (for complier object files)
2. Compile it by make (it has some generic optimization flags now)
3. Run it like

./sir nwk/iceland.lnk 1.5

The first argument is the file containing the network. The input format is an edge list assuming the vertex-id numbers are between 0 and N - 1. The program sets N as one plus the largest index observed, so if your network has isolates make sure at least the largest id has degree > 0. The example network comes from: Haraldsdottir S, Gupta S, Anderson RM, Preliminary studies of sexual networks in a male homosexual community in Iceland, J Acquir Immune Defic Syndr. 1992;5(4):374-81.

The second argument is the per-link infection rate. I assume the recovery rate is one (if some other rate is needed, you just have to divide the infection rate by the recovery rate and divide the output extinction time by the recovery rate).

There can be a third argument: a seed for the RNG = a number between 1 and 2^64-1 = 18446744073709551615

For more comments, see the files. It should a be relatively easy read for a C coder.
