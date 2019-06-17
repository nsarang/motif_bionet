# Motifs in Biological Networks

## Overview

This repo provides an implementation of the paper [Identification of large disjoint motifs in biological networks](https://bmcbioinformatics.biomedcentral.com/articles/10.1186/s12859-016-1271-7). The input is a biological network in the format of the list of edges. The output is a JSON of the same network with motifs being highlighted. A visulization tool is available using d3.js framework.

## Citation

> Elhesha R, Kahveci T. Identification of large disjoint motifs in biological networks. BMC Bioinformatics. 2016;17(1):408.


## Installation
```bash
git clone  https://github.com/nsarang/motif_bionet.git
cd motif_bionet
make
```

## Usage
```
motifnet <inp_path> <out_path> <alpha> <mu>  
```
<pre>
    &lt;inp_path>    Path to the dataset  
    &lt;out_path>    Path to the output file. A JSON file is also created  
    &lt;alpha>       Number of nodes of the motif  
    &lt;mu>          Cut-off of the motif frequency
</pre>
  

**Example**:
motifnet data/cje.txt output/motif 7 5


**Sample output**:
<sub>
Main graph constructed...

BPSf1 created..
M1:	127
M2:	6
M3:	1025
M4:	638

BPS created..
M1:	52
M2:	2
M3:	33
M4:	23

Basic patterns extracted...

Iteration 1		CS size: 7  	outVec size: 0
Iteration 2		CS size: 9  	outVec size: 5
Iteration 3		CS size: 1  	outVec size: 5
Iteration 4		CS size: 0  	outVec size: 5

Finished...
Total running time: 0.32212 seconds.
Max motif frequency: 11

Writing to TXT file...
Writing to JSON file...

Done.
</sub>
