
#ifndef JASPERS_FILE_T_HPP
#define JASPERS_FILE_T_HPP

#include <string>
#include <boost/cstdint.hpp>
#include <utility>

class file_t
{
public:
  typedef ::size_t			size_t; // This can be changed without too much trouble, but in general this should reflect the type of machine the code will run on (if it will run on a 64bit processor and you have a compiler that supports 64bit memory addressing you might want to set it to a 64bit integer).
  typedef boost::int64_t      offset_t; // It is not advisable to change this without reviewing the code below (especially the seek code).
  typedef boost::uint64_t     filesize_t; // This is only used for total file sizes and I'm not completely happy with it.

  enum {
    open_mode_none=0,
    open_mode_read=1<<0,
    open_mode_write=1<<1,
    open_mode_random_access=1<<2,
    open_mode_sequential_access=1<<3,
    open_mode_count=1<<4,
    open_mode_create=1<<5,
  };

  enum seek_base_t {
    seek_base_begin=1,
    seek_base_end=2,
    seek_base_current=3
  };

  enum result_t {
    no_error = 0,

    programming_error,
    unknown_error,

    out_of_memory,
    invalid_handle,

    file_exists,
    file_not_found,
    path_not_found,
    access_denied,
    write_protected,
    invalid_pathname,

    disk_full,
    locked,
    data_corruption,
  };

  static const size_t default_move_buffer_size;
  const size_t move_buffer_size;

  file_t(const size_t move_buffer_size_p=default_move_buffer_size);
  ~file_t();

  bool open(const std::string& file_name_p, const int open_mode);
  bool open(const std::wstring& file_name_p, const int open_mode);
  bool close();
	std::pair<result_t,size_t> read(char*const buffer_p, const size_t buffer_size_p);
  std::pair<result_t,size_t> write(const char*const buffer_p, const size_t buffer_size_p);
  std::pair<result_t,offset_t> seek(const offset_t offset_p, const seek_base_t seek_base_p);
  offset_t tell();
  bool set_eof();
  int open_mode() const { return open_mode_; }; 

  // Utility functions
  bool replace(const size_t old_size_p, const char*const buffer_p, const size_t buffer_size_p);

  static bool exists(const std::string& file_name_p);
  static bool exists(const std::wstring& file_name_p);

  static bool remove(const std::string& file_name_p);
  static bool remove(const std::wstring& file_name_p);

  filesize_t size(); // Returns the file size.

private:
  void* hFile; /**< Only valid if the file is NOT opened with open_mode_count. */
  int open_mode_;
  filesize_t file_pos; /**< Only valid if the file is opened with open_mode_count. */
  filesize_t file_size; /**< Only valid if the file is opened with open_mode_count. */

protected:
  static result_t convert_system_error(int errnum);
};

#endif //JASPERS_FILE_T_HPP
