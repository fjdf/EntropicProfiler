# EntropicProfiler

### Homepage:
http://sels.tecnico.ulisboa.pt/ep/

### Description:
> **Entropic profiles** of DNA sequences are local information plots of the relative over and under-expression of motifs per position.
> They are calculated based on **Chaos Game Representation** (CGR) using a recently proposed _fractal kernel_ and _Parzen's window density estimation method_.
> They allow the visualization of motif densities for two different parameters: _resolution_ __L__ and _smoothing parameter_ __Φ__.
> This method detects biological significant regions of DNA.
> An important simplification allows its calculation using segment counts, explored in this application through **suffix trees**.

### References:
[Vinga, S. and Almeida, J.S.: **Local Renyi entropic profiles of DNA sequences**. BMC Bioinformatics 2007, 8:393.](http://bmcbioinformatics.biomedcentral.com/articles/10.1186/1471-2105-8-393)
[Fernandes F., Vinga S., Freitas A.T., Almeida J.S.: **Entropic Profiler - detection of conservation in genomes using information theory**. BMC Research Notes 2009, 2:72.](http://bmcresnotes.biomedcentral.com/articles/10.1186/1756-0500-2-72)

### Manual:

##### Usage:
```bash
ep -t<type> -f<file> -l<l> -p<phi> -i<position> -m<findmax> -w<window>
```

##### Options:
- `t` : sequence type, should always be 'f', which stands for "file"
- `f` : fasta file to load, should be less than 2 GB, only the 1st sequence inside is loaded, 'N's are converted to 'A's
- `l` : L value (resolution length), should be higher or equal to 3 and less or equal to 10
- `p` : Phi value (smoothing parameter), should be less or equal to 10
- `i` : position to study
- `m` : automatically find the value of L that maximizes the function in that position 'i', should be '0' for false or '1' for true
- `w` : window length to study around that position 'i'
- `x` : (optional parameter) load previously saved project (".tree" file), should be '0' for false or '1' for true
- `y` : (optional parameter) study by motif instead of position, should be a string of DNA characters
- `z` : (optional parameter) get EP values for entire sequence, does not create plots, should be '0' for false or '1' for true

##### Example:
```bash
ep -tf -fExample1.fasta -l8 -p10 -i35840 -m0 -w100
```
