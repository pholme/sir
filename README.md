# sir
Pretty quick code for regular (continuous time, Markovian) SIR on networks

This is code for the susceptible-infected-recovered (SIR) model on simple graphs.

To get it running you need:

1. The Mersenne Twister from here http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/ in a parallel directory
2. Compile it by make (it has some generic optimization flags now)
3. Run it like

./sir nwk/iceland.lnk .rngstate/0 1.5

The first argument is the file containing the network. The input format is an edge list assuming the vertex-id numbers are between 0 and N - 1. The program sets N as one plus the largest index observed, so if your network has isolates make sure at least the largest id has degree > 0. The example network comes from: Haraldsdottir S, Gupta S, Anderson RM, Preliminary studies of sexual networks in a male homosexual community in Iceland, J Acquir Immune Defic Syndr. 1992;5(4):374-81.

The second argument is a RNG state file. If the file does not exist it is generated. When the program exits it overwrites the file with the current state. This arrangement saves a little time of RNG thermalization (or, equivalently, increases the statistic quality vs seeding the RNG by a number). If you want to run the code from the same sequence of random numbers either comment out the line where the state is written to the disk, or copy a state file before every run. If you have many batch jobs in parallel, make sure they each are running with different state files, otherwise reuse the state files as much as possible.

The third argument is the per-link infection rate. I assume the recovery rate is one (if some other rate is needed, you just have to divide the infection rate by the recovery rate and divide the output extinction time by the recovery rate).

For more comments, see the files. It should a be relatively easy read for a C coder.
