
#include "file_t.hpp"
#define LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <boost/shared_ptr.hpp>
#include <limits>

const file_t::size_t file_t::default_move_buffer_size=32768;

file_t::file_t(const size_t move_buffer_size_p) :
  hFile(0),
  move_buffer_size(move_buffer_size_p),
  open_mode_(open_mode_none)
{
  // This will produce an Integer Divide By Zero exception if size_t is smaller than DWORD.
  //static const int test_sizeof_size_t=1/((std::numeric_limits<size_t>::max()>=std::numeric_limits<DWORD>::max())?1:0);
}

file_t::~file_t()
{
  if ( hFile ) close();
}

bool file_t::open(const std::string& file_name_p, const int open_mode)
{
  if ( hFile )
  {
    if ( !close() ) return false;
  }

  if ( !(open_mode & open_mode_count) ) {
    hFile = CreateFileA(
        file_name_p.c_str(),
        ((open_mode & open_mode_write) ? GENERIC_WRITE : 0) | ((open_mode & open_mode_read) ? GENERIC_READ : 0),
        (open_mode & open_mode_write) ? 0 : FILE_SHARE_READ,
        0,
        (open_mode & open_mode_create) ? OPEN_ALWAYS : OPEN_EXISTING,
        ((open_mode & open_mode_write) ? FILE_ATTRIBUTE_ARCHIVE : 0) | ((open_mode & open_mode_sequential_access) ? FILE_FLAG_SEQUENTIAL_SCAN : 0) | ((open_mode & open_mode_random_access) ? FILE_FLAG_RANDOM_ACCESS : 0),
        0
        );

    if ( hFile == INVALID_HANDLE_VALUE ) return false;
  }

  open_mode_=open_mode;

  return true;
}

bool file_t::open(const std::wstring& file_name_p, const int open_mode)
{
  if ( hFile )
  {
    if ( !close() ) return false;
  }

  if ( !(open_mode & open_mode_count) ) {
    hFile = CreateFileW(
        file_name_p.c_str(),
        ((open_mode & open_mode_write) ? GENERIC_WRITE : 0) | ((open_mode & open_mode_read) ? GENERIC_READ : 0),
        (open_mode & open_mode_write) ? 0 : FILE_SHARE_READ,
        0,
        OPEN_ALWAYS,
        ((open_mode & open_mode_write) ? FILE_ATTRIBUTE_ARCHIVE : 0) | ((open_mode & open_mode_sequential_access) ? FILE_FLAG_SEQUENTIAL_SCAN : 0) | ((open_mode & open_mode_random_access) ? FILE_FLAG_RANDOM_ACCESS : 0),
        0
        );

    if ( hFile == INVALID_HANDLE_VALUE ) return false;
  }
  else {
    hFile = 0;
    file_pos = 0;
    file_size = 0;
  }

  open_mode_=open_mode;

  return true;
}

bool file_t::close()
{
  if ( !(open_mode_ & open_mode_count) ) {
    if ( !hFile ) return false;
    const bool return_value=(CloseHandle(hFile)!=0?true:false);
    hFile=0;
    open_mode_=open_mode_none;
    return return_value;
  }
  else {
    /* It's intentional that file_size and file_pos aren't set to zero here, as I rely on being able to later retrieve them. */
    return true;
  }
}

std::pair<file_t::result_t,file_t::size_t> file_t::read(char*const buffer_p, const file_t::size_t buffer_size_p)
{
  if ( !hFile ) return std::make_pair(programming_error, 0);
  size_t to_read=buffer_size_p;
  while(to_read)
  {
    DWORD bufsize=static_cast<DWORD>(std::min(to_read,static_cast<size_t>(std::numeric_limits<DWORD>::max())));
    DWORD bufsize2=0;
    bool success=(ReadFile(hFile, buffer_p+(buffer_size_p-to_read), bufsize, &bufsize2, 0)!=0?true:false);
    to_read-=static_cast<size_t>(bufsize2);
    if ( !success || bufsize2 == 0 )
    {
	    int errnum = GetLastError();
      return std::make_pair(convert_system_error(errnum), buffer_size_p - to_read);
    }
  }
  return std::make_pair(no_error, buffer_size_p);
}


std::pair<file_t::result_t,file_t::size_t> file_t::write(const char*const buffer_p, const file_t::size_t buffer_size_p)
{
  if ( !(open_mode_ & open_mode_count) ) {
    if ( !hFile ) return std::make_pair(programming_error, 0);
    size_t to_write=buffer_size_p;
    while(to_write)
    {
      DWORD bufsize=static_cast<DWORD>(std::min(to_write,static_cast<size_t>(std::numeric_limits<DWORD>::max())));
      DWORD bufsize2=0;
      bool success=(WriteFile(hFile, buffer_p+(buffer_size_p-to_write), bufsize, &bufsize2, 0)!=0?true:false);
      to_write-=static_cast<size_t>(bufsize2);
      if ( !success )
      {
			  int errnum = GetLastError();
        return std::make_pair(convert_system_error(errnum), buffer_size_p - to_write);
      }
    }
  }
  else {
    file_pos += buffer_size_p;
    if ( file_pos > file_size ) {
      file_size = file_pos;
    }
  }
  return std::make_pair(no_error, buffer_size_p);
}

std::pair<file_t::result_t,file_t::offset_t> file_t::seek(const file_t::offset_t offset_p, const seek_base_t seek_base_p)
{
  if ( !(open_mode_ & open_mode_count) ) {
    if ( !hFile ) return std::make_pair(programming_error,static_cast<offset_t>(-1));
    DWORD move_method=0;
    switch(seek_base_p)
    {
    case seek_base_begin:
      move_method=FILE_BEGIN;
      break;
    case seek_base_end:
      move_method=FILE_END;
      break;
    case seek_base_current:
      move_method=FILE_CURRENT;
      break;
    default:
      return std::make_pair(programming_error,tell());
    }
    // This is only valid for LittleEndian systems!
    boost::int32_t offset_low=*(reinterpret_cast<const boost::int32_t*>(&offset_p));
    boost::int32_t offset_high=*(reinterpret_cast<const boost::int32_t*>(&offset_p)+1);
    boost::uint32_t offset_lowret=SetFilePointer(hFile, offset_low, &offset_high, move_method);
	  int errnum = GetLastError();
    if ( offset_lowret == 0xffffffff && errnum != NO_ERROR ) return std::make_pair(convert_system_error(errnum),tell());
    return std::make_pair(no_error,static_cast<offset_t>(offset_lowret) | (static_cast<offset_t>(offset_high)<<32));
  }
  else {
    switch(seek_base_p)
    {
    case seek_base_begin:
      file_pos = offset_p>0 ? offset_p : 0;
      break;
    case seek_base_end:
      file_pos = file_size+offset_p;
      break;
    case seek_base_current:
      file_pos += offset_p;
      break;
    default:
      return std::make_pair(programming_error,tell());
    }
    return std::make_pair(no_error, file_pos);
  }
}

file_t::offset_t file_t::tell()
{
  if ( !(open_mode_ & open_mode_count) ) {
    if ( !hFile ) return -1;
    boost::int32_t offset_high=0;
    boost::uint32_t offset_lowret=SetFilePointer(hFile, 0, &offset_high, FILE_CURRENT);
    return static_cast<offset_t>(offset_lowret) | (static_cast<offset_t>(offset_high)<<32);
  }
  else {
    return file_pos;
  }
}

bool file_t::set_eof()
{
  if ( !(open_mode_ & open_mode_count) ) {
    if ( !hFile ) return false;
    return !!SetEndOfFile(hFile);
  }
  else {
    file_size = file_pos;
    return true;
  }
}

// Utility functions

bool file_t::replace(const file_t::size_t old_size_p, const char*const buffer_p, const file_t::size_t buffer_size_p)
{
  if ( !hFile ) return false;
  if ( old_size_p > buffer_size_p )
  {
    const offset_t old_offset=tell();
    const size_t difference=old_size_p-buffer_size_p;
    const boost::shared_ptr<char> move_buffer(new char[move_buffer_size]);
    if ( !move_buffer ) return false;
    if ( write(buffer_p, buffer_size_p).second != buffer_size_p ) return false;
    while(true)
    {
      seek(difference, seek_base_current);
      const size_t bytes_read=read(move_buffer.get(), move_buffer_size).second;
      seek(0-static_cast<offset_t>(bytes_read)-static_cast<offset_t>(difference), seek_base_current);
      if ( write(move_buffer.get(), bytes_read).second != bytes_read ) return false;
      if ( bytes_read < move_buffer_size ) break; // If this check is incorrect this section of replace will fail (either end up in an endless loop or stop too soon)
    }
    if ( !set_eof() ) return false;
    if ( seek(old_offset, seek_base_begin).second != old_offset ) return false;
  }
  else if ( old_size_p < buffer_size_p )
  {
    const offset_t old_offset=tell();
    const size_t difference=buffer_size_p-old_size_p;
    const boost::shared_ptr<char> move_buffer(new char[move_buffer_size]);
    if ( !move_buffer ) return false;
    if ( seek(0, seek_base_end).first != no_error ) return false;
    size_t to_move=static_cast<size_t>(tell()-(old_offset+static_cast<offset_t>(old_size_p)));
    while(to_move)
    {
      const size_t to_move_cur=std::min(move_buffer_size, to_move);
      seek(old_offset+old_size_p+to_move-to_move_cur, seek_base_begin);
      const size_t bytes_read=read(move_buffer.get(), to_move_cur).second;
      seek(0-static_cast<offset_t>(bytes_read)+static_cast<offset_t>(difference), seek_base_current);
      if ( write(move_buffer.get(), bytes_read).second != bytes_read ) return false;
      to_move-=bytes_read;
    }
    if ( seek(old_offset, seek_base_begin).second != old_offset ) return false;            
    if ( write(buffer_p, buffer_size_p).second != buffer_size_p ) return false;
    if ( seek(old_offset, seek_base_begin).second != old_offset ) return false;
  }
  else
  {
    const offset_t old_offset=tell();
    if ( write(buffer_p, buffer_size_p).second != buffer_size_p ) return false;
    if ( seek(old_offset, seek_base_begin).second != old_offset ) return false;
  }
  return true;
}

bool file_t::exists(const std::string& file_name_p)
{
//return PathFileExistsA(file_name_p.c_str()) == TRUE;
  HANDLE t_hFile = CreateFileA(
      file_name_p.c_str(),
      0,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      0,
      OPEN_EXISTING,
      0,
      0
      );

  if ( t_hFile == INVALID_HANDLE_VALUE ) return false;

  CloseHandle(t_hFile);

  return true;
}

bool file_t::exists(const std::wstring& file_name_p)
{
//return PathFileExistsW(file_name_p.c_str()) == TRUE;
  HANDLE t_hFile = CreateFileW(
      file_name_p.c_str(),
      0,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      0,
      OPEN_EXISTING,
      0,
      0
      );

  if ( t_hFile == INVALID_HANDLE_VALUE ) return false;

  CloseHandle(t_hFile);

  return true;
}

bool file_t::remove(const std::string& file_name_p)
{
	return DeleteFileA(file_name_p.c_str()) != 0;
}

bool file_t::remove(const std::wstring& file_name_p)
{
	return DeleteFileW(file_name_p.c_str()) != 0;
}

file_t::filesize_t file_t::size()
{
  if ( !(open_mode_ & open_mode_count) ) {
    DWORD dwSizeHigh=0;
    DWORD dwSizeLow = GetFileSize(hFile, &dwSizeHigh);  
    if ( dwSizeLow == 0xffffffff && GetLastError() != NO_ERROR ) return -1;
    return static_cast<filesize_t>(dwSizeLow) | (static_cast<filesize_t>(dwSizeHigh)<<32);
  }
  else {
    return file_size;
  }
}

file_t::result_t file_t::convert_system_error(int errnum)
{
	switch(errnum)
	{
	case ERROR_SUCCESS:
		return no_error;

	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_OUTOFMEMORY:
		return out_of_memory;
	case ERROR_INVALID_HANDLE:
		return invalid_handle;

	case ERROR_FILE_EXISTS:
		return file_exists;
	case ERROR_FILE_NOT_FOUND:
		return file_not_found;
	case ERROR_INVALID_DRIVE:
	case ERROR_PATH_NOT_FOUND:
		return path_not_found;
	case ERROR_ACCESS_DENIED:
		return access_denied;
	case ERROR_WRITE_PROTECT:
		return write_protected;
	case ERROR_BAD_PATHNAME:
		return invalid_pathname;

	case ERROR_DISK_FULL:
		return disk_full;
	case ERROR_SHARING_VIOLATION:
	case ERROR_DRIVE_LOCKED:
	case ERROR_LOCK_VIOLATION:
		return locked;
	case ERROR_CRC:
		return data_corruption;

	default:
		return unknown_error;
	}
}
