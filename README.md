# Fast Indexing for Temporal Information Retrieval
<p align="justify">Temporal aspects have received tons of interest in Information Retrieval (IR) and related fields, including database search. The focus of temporal IR is on improving the effectiveness of search by exploiting temporal information in objects and queries. In this work, we study efficient indexing for the fundamental time-travel IR query. Given such a query q with a time interval of interest and a set of descriptive elements (e.g., keywords), the goal is to retrieve all data objects (e.g., documents) whose time interval overlaps with query's and their description contains the elements in q. Existing methods extend the inverted index to answer time-travel IR queries, with simple but ineffective temporal indexing. We propose new methods which capitalize on the state-of-the-art interval index HINT in two ways; either by extending again the inverted index or adopting the time-first irHINT approach which directly builds on HINT. Our experiments showed that irHINT outperforms all IR-first methods, while exhibiting good indexing and updating costs.</p>


## Dependencies
- g++/gcc
- Boost Library 


## Data
All datasets (and the queries of different extent, element count, and element selectivity) used in the experiments can be sourced from [https://seafile.rlp.net/d/cf3ecc113a7c4c63baa7](https://seafile.rlp.net/d/cf3ecc113a7c4c63baa7). Put the unzipped files into directory ```samples```.
| Dataset | Archive | Uncompressed |
| ------ | ------ | ------ |
| ECLOG | 88.6 MB | 241.1 MB |
| WIKIPEDIA | 291.7 MB | 6.3 GB |
| SYNTHETICS | 3.0 GB | 9.5 GB |


## Compile
Compile using ```make all``` or ```make <option>``` where `<option>` can be one of the following:
- lscan 
- tif
- irhint
- tif_u
- irhint_u


## Shared parameters among all methods
| Parameter | Description | Comment |
| ------ | ------ | ------ |
| -? or -h | display help message | |
| -v | activate verbose mode; print the trace for every query; otherwise only the final report is displayed | |
| -r | set the number of runs per query; by default 1 | in our experimental analysis set to 10 |


## Indexing and query processing methods

### Linear scan:

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- main_lscan.cpp

#### Execution

- ##### Examples
    ```sh
    $ ./query_lscan.exec samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### tIF+Slicing

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- indices/inverted_file.h
- indices/inverted_file.cpp
- indices/1dgrid.cpp
- indices/temporal_inverted_file.h
- indices/temporal_inverted_file.cpp
- indices/temporal_inverted_file_slicing.h
- indices/temporal_inverted_file_slicing.cpp
- main_tif.cpp
- main_tif_update.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -o | \"SLICING\" to activate slicing of posting lists |  |
| -p | set the number of partitions | 50 for ECLOG, WIKIPEDIA, and 250 for SYNTHETICS |

- ##### Examples
    ```sh
    $ ./query_tif.exec -o SLICING -p 50 samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### tIF+Sharding

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- indices/inverted_file.h
- indices/inverted_file.cpp
- indices/sharded_postinglist.h
- indices/sharded_postinglist.cpp
- indices/temporal_inverted_file.h
- indices/temporal_inverted_file.cpp
- indices/temporal_inverted_file_sharding.h
- indices/temporal_inverted_file_sharding.cpp
- main_tif.cpp
- main_tif_update.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -o | \"SHARDING\" to activate sharding of posting lists |  |
| -x | set the relaxation factor in percent of the domain | 5 for ECLOG, 2 for both WIKIPEDIA and SYNTHETICS |
| -i | set the gap for impact-list entries | 100 for ECLOG, 200 for both WIKIPEDIA and SYNTHETICS |

- ##### Examples
    ```sh
    $ ./query_tif.exec -o SHARDING -x 5 -i 100 samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### tIF+HINT using binary search variant (a.k.a. tIF+HINT-beta)

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- containers/offsets.h
- containers/offsets.cpp
- indices/hierarchical_index.h
- indices/hierarchical_index.cpp
- indices/inverted_file.h
- indices/inverted_file.cpp
- indices/temporal_inverted_file.h
- indices/temporal_inverted_file.cpp
- indices/temporal_inverted_file_hint_beta.h
- indices/temporal_inverted_file_hint_beta.cpp
- main_tif.cpp
- main_tif_update.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -o | \"HINTB\" to activate HINT-organized posting lists with binary search |  |
| -m | set the number of bits; if not set, a value will be automatically determined using the cost model | 10 for both ECLOG and WIKIPEDIA |

- ##### Examples
    ```sh
    $ ./query_tif.exec -o HINTB -m 10 samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### tIF+HINT using merge-sort variant (a.k.a. tIF+HINT-gamma)

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- containers/offsets.h
- containers/offsets.cpp
- indices/hierarchical_index.h
- indices/hierarchical_index.cpp
- indices/inverted_file.h
- indices/inverted_file.cpp
- indices/temporal_inverted_file.h
- indices/temporal_inverted_file.cpp
- indices/temporal_inverted_file_hint_gamma.h
- indices/temporal_inverted_file_hint_gamma.cpp
- main_tif.cpp
- main_tif_update.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -o | \"HINTG\" to activate HINT-organized posting lists with merge sort |  |
| -m | set the number of bits; if not set, a value will be automatically determined using the cost model | 5 for both ECLOG and WIKIPEDIA, and 8 for SYNTHETICS |

- ##### Examples
    ```sh
    $ ./query_tif.exec -o HINTG -m 5 samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### tIF+HINT+Slicing hybrid variant 

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- containers/offsets.h
- containers/offsets.cpp
- indices/hierarchical_index.h
- indices/hierarchical_index.cpp
- indices/inverted_file.h
- indices/inverted_file.cpp
- indices/temporal_inverted_file.h
- indices/temporal_inverted_file.cpp
- indices/temporal_inverted_file_hint_gamma.h
- indices/temporal_inverted_file_hint_gamma.cpp
- main_tif.cpp
- main_tif_update.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -o | \"HINTSLICING\" to activate HINT-organized posting lists with Slicing |  |
| -m | set the number of bits; if not set, a value will be automatically determined using the cost model | 5 for both ECLOG and WIKIPEDIA, and 8 for SYNTHETICS |
| -p | set the number of partitions | 50 for both ECLOG and WIKIPEDIA, and 250 for SYNTHETICS |

- ##### Examples
    ```sh
    $ ./query_tif.exec -o HINTSLICING -m 5 -p 50 samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### irHINT performance variant (a.k.a. irHINT-alpha)

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- containers/offsets.h
- containers/offsets.cpp
- indices/hierarchical_index.h
- indices/hierarchical_index.cpp
- indices/irhint.h
- indices/irhint.cpp
- main_irhint.cpp
- main_irhint_update.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -o | \"A\" to activate performance mode |  |
| -m | set the number of bits; if not set, a value will be automatically determined using the cost model | 10 for ECLOG, 9 for WIKIPEDIA and 13 for SYNTHETICS |

- ##### Examples
    ```sh
    $ ./query_irhint.exec -o A -m 10 samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### irHINT size variant (a.k.a. irHINT-beta)

#### Source code files
- def_global.h
- utils.cpp
- containers/relations.h
- containers/relations.cpp
- containers/offsets.h
- containers/offsets.cpp
- indices/hierarchical_index.h
- indices/hierarchical_index.cpp
- indices/irhint.h
- indices/irhint.cpp
- main_irhint.cpp
- main_irhint_update.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -o | \"B\" to activate memory mode |  |
| -m | set the number of bits; if not set, a value will be automatically determined using the cost model | 10 for both ECLOG and WIKIPEDIA and 15 for SYNTHETICS |

- ##### Examples
    ```sh
    $ ./query_irhint.exec -o B samples/eclog/ECOM-LOG.dat samples/eclog/ECOM-LOG.dat_10K_elems3-extent0.1%.qry
    ```


### Updates
Call update_tif.exec or update_irhint.exec with two datasets.
The first will be used for constructing, the second for updating the index.

- ##### Examples
    ```sh
    $ ./update_tif.exec -o HINTSLICING -m 5 -p 50 samples/eclog/ECOM-LOG.dat_bottom90% samples/eclog/ECOM-LOG.dat_top10%
    ```
    ```sh
    $ ./query_irhint.exec -o A -m 10 samples/eclog/ECOM-LOG.dat_bottom90% samples/eclog/ECOM-LOG.dat_top10%
    ```


## Experiments
To reproduce all experiments in the paper follow the instructions below.
Reports will be written to ```output_<dataset>```; one file for each dataset.

### Real datasets
Execute experiments:
- vary query extent
- vary query elements
- vary selectivity
- vary update percentage

```
$ bash scripts/experiments_real.sh
```

### Synthetic datasets
Execute experiments:
- vary cardinality
- vary domain
- vary dictionary
- vary alpha (interval duration)
- vary description
- vary deviation (interval position)
- vary skewness (element frequency)
- vary query extent
- vary query elements

```
$ bash scripts/experiments_synthetic.sh
```
