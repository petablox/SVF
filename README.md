## News
* <b>SVF now uses a single script for its build. Just type [`./build.sh`](https://github.com/SVF-tools/SVF/blob/master/build.sh) in your terminal, that's it!</b>
* <b>SVF now supports LLVM-10.0.0! </b>
* <b>We thank [bsauce](https://github.com/bsauce) for writing a user manual of SVF ([link1](https://www.jianshu.com/p/068a08ec749c) and [link2](https://www.jianshu.com/p/777c30d4240e)) in Chinese </b>
* <b>SVF now supports LLVM-9.0.0 (Thank [Byoungyoung Lee](https://github.com/SVF-tools/SVF/issues/142) for his help!). </b>
* <b>SVF now supports a set of [field-sensitive pointer analyses](https://yuleisui.github.io/publications/sas2019a.pdf). </b>
* <b>[Use SVF as an external lib](https://github.com/SVF-tools/SVF/wiki/Using-SVF-as-a-lib-in-your-own-tool) for your own project (Contributed by [Hongxu Chen](https://github.com/HongxuChen)). </b>
* <b>SVF now supports LLVM-7.0.0. </b>
* <b>SVF now supports Docker. [Try SVF in Docker](https://github.com/SVF-tools/SVF/wiki/Try-SVF-in-Docker)! </b>
* <b>SVF now supports [LLVM-6.0.0](https://github.com/svf-tools/SVF/pull/38) (Contributed by [Jack Anthony](https://github.com/jackanth)). </b>
* <b>SVF now supports [LLVM-4.0.0](https://github.com/svf-tools/SVF/pull/23) (Contributed by Jared Carlson. Thank [Jared](https://github.com/jcarlson23) and [Will](https://github.com/dtzWill) for their in-depth [discussions](https://github.com/svf-tools/SVF/pull/18) about updating SVF!) </b>
* <b>SVF now supports analysis for C++ programs.</b>
<br />

#### We are looking for self-motivated PhD students and we welcome industry collaboration/sponsorship to improve SVF (Please contact yulei.sui@uts.edu.au if you are interested)


<br />
<br />
<br />
SVF is a static tool that enables scalable and precise interprocedural dependence analysis for C and C++ programs. SVF allows value-flow construction and pointer analysis to be performed iteratively, thereby providing increasingly improved precision for both. 

SVF accepts the points-to information generated by any pointer analysis (e.g., Andersen’s analysis) and constructs an interprocedural memory SSA form so that the def-use chains of both top-level and address-taken variables are captured. SVF is implemented on top of an industry-strength compiler [LLVM](http://llvm.org) (version 6.0.0). SVF contains a third party software package [CUDD-2.5.0](http://vlsi.colorado.edu/~fabio/CUDD/) (Binary Decision Diagrams (BDDs)), which is used to encode path conditions.

<br />

| About SVF       | Setup  Guide         | User Guide  | Developer Guide  |
| ------------- |:-------------:| -----:|-----:|
| ![About](https://github.com/svf-tools/SVF/blob/master/images/help.png?raw=true)| ![Setup](https://github.com/svf-tools/SVF/blob/master/images/tools.png?raw=true)  | ![User](https://github.com/svf-tools/SVF/blob/master/images/users.png?raw=true)  |  ![Developer](https://github.com/svf-tools/SVF/blob/master/images/database.png?raw=true) 
| Introducing SVF -- [what it does](https://github.com/svf-tools/SVF/wiki/About#what-is-svf) and [how we design it](https://github.com/svf-tools/SVF/wiki/SVF-Design#svf-design)      | A step by step [setup guide](https://github.com/svf-tools/SVF/wiki/Setup-Guide#getting-started) to build SVF | Command-line options to [run SVF](https://github.com/svf-tools/SVF/wiki/User-Guide#quick-start), get [analysis outputs](https://github.com/svf-tools/SVF/wiki/User-Guide#analysis-outputs), and test SVF with [an example](https://github.com/svf-tools/SVF/wiki/Analyze-a-Simple-C-Program) or [PTABen](https://github.com/SVF-tools/PTABen) | Detailed [technical documentation](https://github.com/svf-tools/SVF/wiki/Technical-documentation) and how to [write your own analyses](https://github.com/svf-tools/SVF/wiki/Write-your-own-analysis-in-SVF) in SVF or [use SVF as a lib](https://github.com/SVF-tools/SVF/wiki/Using-SVF-as-a-lib-in-your-own-tool) for your tool  |


<br />
<br />
<p>We release SVF source code in the hope of benefiting others. You are kindly asked to acknowledge usage of the tool by citing some of our publications listed http://svf-tools.github.io/SVF, especially the following two: </p>

```
@inproceedings{sui2016svf,
  title={SVF: interprocedural static value-flow analysis in LLVM},
  author={Sui, Yulei and Xue, Jingling},
  booktitle={Proceedings of the 25th international conference on compiler construction},
  pages={265--266},
  year={2016},
  organization={ACM}
}
```

```
@article{sui2014detecting,
  title={Detecting memory leaks statically with full-sparse value-flow analysis},
  author={Sui, Yulei and Ye, Ding and Xue, Jingling},
  journal={IEEE Transactions on Software Engineering},
  volume={40},
  number={2},
  pages={107--122},
  year={2014},
  publisher={IEEE}
}
```

## Petablox Additions

### SVF Reachability

Our static reachability tool is implemented at `tools/Reach`. To build, execute the SVF build process above. Once built, you can locate the executable at `BUILD_DIR/bin/svf-reach`.

To use the tool, we require a `.bc` file for both the executable we want to calculate reachability from, and all of its dependendant libraries. Our [dep-trace](https://github.com/petablox/dep-trace/tree/better-symbols) tool aids in building a `.bc` for all of an application's dependencies. Note that for the tool calculates reachability per linkage module, which mostly corresponds to a shared library. 

Currently, on `fir07`, we have pre-built `.bc` files for a few packages. `/data3/pkg-debloating/wllvm-build/wget-src-out/wllvm-bc/` contains `.bc` files for `wget` and its dependencies. The following is an example to use the tool to calculate dependencies reachable from `wget` main function:

```
BUILD_DIR/bin/svf-reach -t type $HOME/dep-trace/srcs/wllvm-build/wget-src-out/wllvm-bc/*.b
```

The above uses class-hierarchy analysis (-t type) option. To use the more precise but slower andersen wave difference analysis, pass the `anders` option instead:

```
BUILD_DIR/bin/svf-reach -t anders $HOME/dep-trace/srcs/wllvm-build/wget-src-out/wllvm-bc/*.bc
```

Lastly, because libraries opened with `dlopen` and invoked with `dlsym` are tricky to trace with static analysis, we have a separate way to trace their usage. We create a module file that includes a list of bc files (shared libraries) where each library will be used as a starting point (including the main function).  Each line in the file should name a `.bc` module with a path as the same path used as input to the tool.  For example, lets say we create `m1.txt`:

```
$HOME/dep-trace/srcs/wllvm-build/wget-src-out/wllvm-bc/libacl.so.1.bc
$HOME/dep-trace/srcs/wllvm-build/wget-src-out/wllvm-bc/libidn2.so.0.bc
```
And run as:

```
BUILD_DIR/bin/svf-reach -t type -m m1.txt $HOME/dep-trace/srcs/wllvm-build/wget-src-out/wllvm-bc/*.bc
```
