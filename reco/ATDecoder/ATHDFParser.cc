#define _ATHDFPARSER_CC_
#include "ATHDFParser.hh"

#define cRED "\033[1;31m"
#define cYELLOW "\033[1;33m"
#define cNORMAL "\033[0m"
#define cGREEN "\033[1;32m"

ClassImp(ATHDFParser);

class ATHDFParser::ATHDFParser_impl{

  public:
    ATHDFParser_impl() {}
    ~ATHDFParser_impl() {}

    hid_t _file;
    hid_t _group;
    hid_t _dataset;
  };

  ATHDFParser::ATHDFParser() : _pimpl{new ATHDFParser_impl{}, [](ATHDFParser_impl* ptr) { delete ptr; }} {}

  hid_t ATHDFParser::open_file(char const* file, IO_MODE mode)
  {
    hid_t fileId;
    (mode == IO_MODE::READ) ?
      fileId = H5Fopen(file, H5F_ACC_RDONLY, H5P_DEFAULT) :
      fileId = H5Fopen(file, H5F_ACC_RDWR, H5P_DEFAULT);

    if (fileId >= 0)
    {
      //std::cout << "> hdf5_wrapper::open_file:MESSAGE, opening file: " << file << ", mode: " << mode << ", ID: " << fileId << '\n';
      return fileId;
    }
    else
    {
      std::cerr << "> ATHDFParser::open_file:ERROR, invalid ID for file: " << file << '\n';
      return 0;
    }
  }

  std::tuple<hid_t, hsize_t> ATHDFParser::open_group(hid_t fileId, char const* group)
  {
    hid_t groupId = H5Gopen2(fileId, group, H5P_DEFAULT);
    if (groupId >= 0)
    {
      //std::cout << "> hdf5_wrapper::open_group:MESSAGE, opening group: " << group << ", ID: " << groupId << '\n';
      hsize_t size;
      H5Gget_num_objs(groupId, &size);
      return std::make_tuple(groupId, size);
    }
    else
    {
      std::cerr << "> ATHDFParser::open_group:ERROR, invalid ID for group: " << group << '\n';
      return std::make_tuple(-1, -1);
    }
  }

  std::tuple<hid_t, std::vector<hsize_t> > ATHDFParser::open_dataset(hid_t locId, char const* dataset)
  {
    hid_t datasetId = H5Dopen2(locId, dataset, H5P_DEFAULT);
    if (datasetId >= 0)
    {
      //std::cout << "> hdf5_wrapper::open_dataset:MESSAGE, opening dataset: " << dataset << ", ID: " << datasetId << '\n';
      hid_t dspaceId = H5Dget_space(datasetId);
      int n_dims = H5Sget_simple_extent_ndims(dspaceId);
      hsize_t dims[n_dims];
      H5Sget_simple_extent_dims(dspaceId, dims, nullptr);
      std::vector<hsize_t> v_dims(dims,dims + n_dims); 
      return std::make_tuple(datasetId,v_dims);
    }
    else
    {
      std::cerr << "> ATHDFParser::open_dataset:ERROR, invalid ID for dataset: " << dataset << '\n';
      std::vector<hsize_t> v{0};
      return std::make_tuple(0,v);
    }
  }

  void ATHDFParser::close_file(hid_t fileId)
  {
    herr_t retId = H5Fclose(fileId);
    if (retId < 0)
      std::cerr << "> ATHDFParser::close_file:ERROR, cannot close file with ID: " << fileId << '\n';
  }

  void ATHDFParser::close_group(hid_t groupId)
  {
    herr_t retId = H5Gclose(groupId);
    if (retId < 0)
      std::cerr << "> ATHDFParser::close_group:ERROR, cannot close group with ID: " << groupId << '\n';
  }

  void ATHDFParser::close_dataset(hid_t datasetId)
  {
    herr_t retId = H5Dclose(datasetId);
    if (retId < 0)
      std::cerr << "> ATHDFParser::close_dataset:ERROR, cannot close dataset with ID: " << datasetId << '\n';
  }

  std::size_t ATHDFParser::open(char const* file)
  {
    auto f = open_file(file, ATHDFParser::IO_MODE::READ);
    if (f==0) return 0;
    _pimpl->_file = f;
    auto group_n_entries = open_group(f, "get");
    if (std::get<0>(group_n_entries)==-1) return 0;
    _pimpl->_group = std::get<0>(group_n_entries);
    return std::get<1>(group_n_entries);
  }

  std::size_t ATHDFParser::n_pads(std::size_t i_raw_event)
  {
    std::string dataset_name = std::to_string(i_raw_event);
    auto dataset_dims = open_dataset(_pimpl->_group, dataset_name.c_str());
    if (std::get<0>(dataset_dims)==0) return 0;
    _pimpl->_dataset = std::get<0>(dataset_dims);
    return std::get<1>(dataset_dims)[0];
  }

  std::vector<int16_t> ATHDFParser::pad_raw_data(std::size_t i_pad)
  {
    int16_t data[517];
    hsize_t counts[2] = {1, 517};
    hsize_t offsets[2] = {i_pad, 0};
    hsize_t dims_out[2] = {1, 517};
    read_slab<int16_t>(_pimpl->_dataset, counts, offsets, dims_out, data);
    std::vector<int16_t> datav(data,data+517);
    return datav;
  }

  void ATHDFParser::end_raw_event()
  {
    close_dataset(_pimpl->_dataset);
  }

  void ATHDFParser::close()
  {
    close_group(_pimpl->_group);
    close_file(_pimpl->_file);
  }


