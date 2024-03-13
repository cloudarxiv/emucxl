## emucxl: an emulation framework for CXL-based disaggregated memory applications

This repository contains the source code and scripts to reproduce the experiments in the paper "emucxl: an emulation framework for CXL-based disaggregated memory applications".

#### emucxl setup instructions
emucxl uses the [pond](https://github.com/vtess/Pond) techniques for cxl emulation. 
Refer to the [emucxl setup README](https://github.com/cloudarxiv/pond-emucxl/tree/master/emucxl) for setting up emucxl on a regular 2-socket (2S) system. Run the following command to start the VM.

```bash
    $ bash ./scripts/start_vm.sh
    # you can configure the remote and local memory size in the start_vm.sh script
```

#### Reproduce experiments results
```bash
    $ cd scripts
    $ bash ./experiments.sh
    # The experiments.sh script will run the experiments and generate the results. Results will be stored in the results directory.
```