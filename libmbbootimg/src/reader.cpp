/*
 * Copyright (C) 2017  Andrew Gunnerson <andrewgunnerson@gmail.com>
 *
 * This file is part of DualBootPatcher
 *
 * DualBootPatcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DualBootPatcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DualBootPatcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mbbootimg/reader.h"

#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mbcommon/file.h"
#include "mbcommon/file/standard.h"
#include "mbcommon/string.h"

#include "mbbootimg/entry.h"
#include "mbbootimg/header.h"

#define ENSURE_STATE_OR_RETURN(STATES, RETVAL) \
    do { \
        if (!(m_state & (STATES))) { \
            set_error(ReaderError::InvalidState, \
                      "%s: Invalid state: "\
                      "expected 0x%" PRIx8 ", actual: 0x%" PRIx8, \
                      __func__, static_cast<uint8_t>(STATES), \
                      static_cast<uint8_t>(m_state)); \
            return RETVAL; \
        } \
    } while (0)

/*!
 * \file mbbootimg/reader.h
 * \brief Boot image reader API
 */

/*!
 * \file mbbootimg/reader_p.h
 * \brief Boot image reader private API
 */

/*!
 * \defgroup MB_BI_READER_FORMAT_CALLBACKS Format reader callbacks
 */

/*!
 * \typedef FormatReaderBidder
 * \ingroup MB_BI_READER_FORMAT_CALLBACKS
 *
 * \brief Format reader callback to place bid
 *
 * Place a bid based on the confidence in which the format reader can parse the
 * boot image. The bid is usually the number of bits the reader is confident
 * that conform to the file format (eg. magic string). The file position will be
 * set to the beginning of the file before this function is called.
 *
 * \param bir MbBiReader
 * \param userdata User callback data
 * \param best_bid Current best bid
 *
 * \return
 *   * Return a non-negative integer to place a bid
 *   * Return #RET_WARN to indicate that the bid cannot be won
 *   * Return \<= #RET_FAILED if an error occurs.
 */

/*!
 * \typedef FormatReaderSetOption
 * \ingroup MB_BI_READER_FORMAT_CALLBACKS
 *
 * \brief Format reader callback to set option
 *
 * \param bir MbBiReader
 * \param userdata User callback data
 * \param key Option key
 * \param value Option value
 *
 * \return
 *   * Return #RET_OK if the option is handled successfully
 *   * Return #RET_WARN if the option cannot be handled
 *   * Return \<= #RET_FAILED if an error occurs
 */

/*!
 * \typedef FormatReaderReadHeader
 * \ingroup MB_BI_READER_FORMAT_CALLBACKS
 *
 * \brief Format reader callback to read header
 *
 * \param[in] bir MbBiReader
 * \param[in] userdata User callback data
 * \param[out] header Header instance to write header values
 *
 * \return
 *   * Return #RET_OK if the header is successfully read
 *   * Return \<= #RET_WARN if an error occurs
 */

/*!
 * \typedef FormatReaderReadEntry
 * \ingroup MB_BI_READER_FORMAT_CALLBACKS
 *
 * \brief Format reader callback to read next entry
 *
 * \note This callback *must* be able to skip to the next entry if the user does
 *       not read or finish reading the entry data with Reader::read_data().
 *
 * \param[in] bir MbBiReader
 * \param[in] userdata User callback data
 * \param[out] entry Entry instance to write entry values
 *
 * \return
 *   * Return #RET_OK if the entry is successfully read
 *   * Return \<= #RET_WARN if an error occurs
 */

/*!
 * \typedef FormatReaderGoToEntry
 * \ingroup MB_BI_READER_FORMAT_CALLBACKS
 *
 * \brief Format reader callback to seek to a specific entry
 *
 * \param[in] bir MbBiReader
 * \param[in] userdata User callback data
 * \param[out] entry Entry instance to write entry values
 * \param[in] entry_type Entry type to seek to
 *
 * \return
 *   * Return #RET_OK if the entry is successfully read
 *   * Return #RET_EOF if the entry cannot be found
 *   * Return \<= #RET_WARN if an error occurs
 */

/*!
 * \typedef FormatReaderReadData
 * \ingroup MB_BI_READER_FORMAT_CALLBACKS
 *
 * \brief Format reader callback to read entry data
 *
 * \note This function *may* return less than \p buf_size bytes, but *must*
 *       return more than 0 bytes unless EOF is reached.
 *
 * \param[in] bir MbBiReader
 * \param[in] userdata User callback data
 * \param[out] buf Output buffer to write data
 * \param[in] buf_size Size of output buffer
 * \param[out] bytes_read Output number of bytes that are read
 *
 * \return
 *   * Return #RET_OK if the entry is successfully read
 *   * Return #RET_EOF if the end of the curent entry has been reached
 *   * Return \<= #RET_WARN if an error occurs
 */

///

namespace mb
{
namespace bootimg
{

using namespace detail;

static struct
{
    int code;
    const char *name;
    int (Reader::*func)();
} g_reader_formats[] = {
    {
        FORMAT_ANDROID,
        FORMAT_NAME_ANDROID,
        &Reader::enable_format_android
    }, {
        FORMAT_BUMP,
        FORMAT_NAME_BUMP,
        &Reader::enable_format_bump
    }, {
        FORMAT_LOKI,
        FORMAT_NAME_LOKI,
        &Reader::enable_format_loki
    }, {
        FORMAT_MTK,
        FORMAT_NAME_MTK,
        &Reader::enable_format_mtk
    }, {
        FORMAT_SONY_ELF,
        FORMAT_NAME_SONY_ELF,
        &Reader::enable_format_sony_elf
    },
};

FormatReader::FormatReader(Reader &reader)
    : _reader(reader)
{
}

FormatReader::~FormatReader() = default;

int FormatReader::init()
{
    return RET_OK;
}

int FormatReader::set_option(const char *key, const char *value)
{
    (void) key;
    (void) value;
    return RET_OK;
}

int FormatReader::go_to_entry(File &file, Entry &entry, int entry_type)
{
    (void) file;
    (void) entry;
    (void) entry_type;
    return RET_UNSUPPORTED;
}

/*!
 * \brief Construct new Reader.
 */
Reader::Reader()
    : m_state(ReaderState::New)
    , m_owned_file()
    , m_file()
    , m_format()
{
}

/*!
 * \brief Destroy a Reader.
 *
 * If the reader has not been closed, it will be closed. Since this is the
 * destructor, it is not possible to get the result of the close operation. To
 * get the result of the close operation, call Reader::close() manually.
 */
Reader::~Reader()
{
    if (m_state != ReaderState::Moved
            && m_state != ReaderState::Closed) {
        close();
    }
}

Reader::Reader(Reader &&other) noexcept
    : m_state(other.m_state)
    , m_owned_file(std::move(other.m_owned_file))
    , m_file(other.m_file)
    , m_error_code(other.m_error_code)
    , m_error_string(std::move(other.m_error_string))
    , m_formats(std::move(other.m_formats))
    , m_format(other.m_format)
{
    other.m_state = ReaderState::Moved;
}

Reader & Reader::operator=(Reader &&rhs) noexcept
{
    (void) close();

    m_state = rhs.m_state;
    m_owned_file.swap(rhs.m_owned_file);
    m_file = rhs.m_file;
    m_error_code = rhs.m_error_code;
    m_error_string.swap(rhs.m_error_string);
    m_formats.swap(rhs.m_formats);
    m_format = rhs.m_format;

    rhs.m_state = ReaderState::Moved;

    return *this;
}

/*!
 * \brief Open boot image from filename (MBS).
 *
 * \param filename MBS filename
 *
 * \return
 *   * #RET_OK if the boot image is successfully opened
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::open_filename(const std::string &filename)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);

    auto file = std::make_unique<StandardFile>();
    auto ret = file->open(filename, FileOpenMode::ReadOnly);

    if (!ret) {
        set_error(ret.error(),
                  "Failed to open for reading: %s",
                  ret.error().message().c_str());
        return RET_FAILED;
    }

    return open(std::move(file));
}

/*!
 * \brief Open boot image from filename (WCS).
 *
 * \param filename WCS filename
 *
 * \return
 *   * #RET_OK if the boot image is successfully opened
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::open_filename_w(const std::wstring &filename)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);

    auto file = std::make_unique<StandardFile>();
    auto ret = file->open(filename, FileOpenMode::ReadOnly);

    if (!ret) {
        set_error(ret.error(),
                  "Failed to open for reading: %s",
                  ret.error().message().c_str());
        return RET_FAILED;
    }

    return open(std::move(file));
}

/*!
 * \brief Open boot image from File handle.
 *
 * This function will take ownership of the file handle. When the Reader is
 * closed, the file handle will also be closed.
 *
 * \param file File handle
 *
 * \return
 *   * #RET_OK if the boot image is successfully opened
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::open(std::unique_ptr<File> file)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);

    int ret = open(file.get());
    if (ret != RET_OK) {
        return ret;
    }

    // Underlying pointer is not invalidated during a move
    m_owned_file = std::move(file);
    return RET_OK;
}

/*!
 * \brief Open boot image from File handle.
 *
 * This function will not take ownership of the file handle. When the Reader is
 * closed, the file handle will remain open.
 *
 * \param file File handle
 *
 * \return
 *   * #RET_OK if the boot image is successfully opened
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::open(File *file)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);

    int best_bid = 0;
    FormatReader *format = nullptr;

    if (m_formats.empty()) {
        set_error(ReaderError::NoFormatsRegistered);
        return RET_FAILED;
    }

    // Perform bid if a format wasn't explicitly chosen
    if (!m_format) {
        for (auto &f : m_formats) {
            // Seek to beginning
            auto seek_ret = file->seek(0, SEEK_SET);
            if (!seek_ret) {
                set_error(seek_ret.error(),
                          "Failed to seek file: %s",
                          seek_ret.error().message().c_str());
                return file->is_fatal() ? RET_FATAL : RET_FAILED;
            }

            // Call bidder
            int ret = f->bid(*file, best_bid);
            if (ret > best_bid) {
                best_bid = ret;
                format = f.get();
            } else if (ret == RET_WARN) {
                continue;
            } else if (ret < 0) {
                return ret;
            }
        }

        if (!format) {
            set_error(ReaderError::UnknownFileFormat,
                      "Failed to determine boot image format");
            return RET_FAILED;
        }

        m_format = format;
    }

    m_state = ReaderState::Header;
    m_file = file;

    return RET_OK;
}

/*!
 * \brief Close a Reader.
 *
 * This function will close a Reader if it is open. Regardless of the return
 * value, the reader is closed and can no longer be used for further operations.
 *
 * \return
 *   * #RET_OK if the reader is successfully closed
 *   * \<= #RET_WARN if the reader is opened and an error occurs while
 *     closing the reader
 */
int Reader::close()
{
    ENSURE_STATE_OR_RETURN(~ReaderStates(ReaderState::Moved), RET_FATAL);

    int ret = RET_OK;

    // Avoid double-closing or closing nothing
    if (!(m_state & (ReaderState::Closed | ReaderState::New))) {
        if (m_owned_file) {
            if (!m_owned_file->close()) {
                if (RET_FAILED < ret) {
                    ret = RET_FAILED;
                }
            }
        }

        m_owned_file.reset();
        m_file = nullptr;

        // Don't change state to ReaderState::FATAL if RET_FATAL is returned.
        // Otherwise, we risk double-closing the boot image. CLOSED and FATAL
        // are the same anyway, aside from the fact that boot images can be
        // closed in the latter state.
    }

    m_state = ReaderState::Closed;

    return ret;
}

/*!
 * \brief Read boot image header.
 *
 * Read the header from the boot image and store the header values to a Header.
 *
 * \param[out] header Reference to Header for storing header values
 *
 * \return
 *   * #RET_OK if the boot image header is successfully read
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::read_header(Header &header)
{
    ENSURE_STATE_OR_RETURN(ReaderState::Header, RET_FATAL);

    // Seek to beginning
    auto seek_ret = m_file->seek(0, SEEK_SET);
    if (!seek_ret) {
        set_error(seek_ret.error(),
                  "Failed to seek file: %s",
                  seek_ret.error().message().c_str());
        return m_file->is_fatal() ? RET_FATAL : RET_FAILED;
    }

    header.clear();

    int ret = m_format->read_header(*m_file, header);
    if (ret == RET_OK) {
        m_state = ReaderState::Entry;
    } else if (ret <= RET_FATAL) {
        m_state = ReaderState::Fatal;
    }

    return ret;
}

/*!
 * \brief Read next boot image entry.
 *
 * Read the next entry from the boot image and store the entry values to an
 * Entry.
 *
 * \param[out] entry Reference to Entry for storing entry values
 *
 * \return
 *   * #RET_OK if the boot image entry is successfully read
 *   * #RET_EOF if the boot image has no more entries
 *   * #RET_UNSUPPORTED if the boot image format does not support seeking
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::read_entry(Entry &entry)
{
    // Allow skipping to the next entry without reading the data
    ENSURE_STATE_OR_RETURN(ReaderState::Entry | ReaderState::Data, RET_FATAL);

    entry.clear();

    int ret = m_format->read_entry(*m_file, entry);
    if (ret == RET_OK) {
        m_state = ReaderState::Data;
    } else if (ret <= RET_FATAL) {
        m_state = ReaderState::Fatal;
    }

    return ret;
}

/*!
 * \brief Seek to specific boot image entry.
 *
 * Seek to the specified entry in the boot image and store the entry values to
 * an Entry.
 *
 * \param[out] entry Reference to Entry for storing entry values
 * \param[in] entry_type Entry type to seek to (0 for first entry)
 *
 * \return
 *   * #RET_OK if the boot image entry is successfully read
 *   * #RET_EOF if the boot image entry is not found
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::go_to_entry(Entry &entry, int entry_type)
{
    // Allow skipping to an entry without reading the data
    ENSURE_STATE_OR_RETURN(ReaderState::Entry | ReaderState::Data, RET_FATAL);

    entry.clear();

    int ret = m_format->go_to_entry(*m_file, entry, entry_type);
    if (ret == RET_OK) {
        m_state = ReaderState::Data;
    } else if (ret <= RET_FATAL) {
        m_state = ReaderState::Fatal;
    }

    return ret;
}

/*!
 * \brief Read current boot image entry data.
 *
 * \param[out] buf Output buffer
 * \param[in] size Size of output buffer
 * \param[out] bytes_read Pointer to store number of bytes read
 *
 * \return
 *   * #RET_OK if data is successfully read
 *   * #RET_EOF if EOF is reached for the current entry
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::read_data(void *buf, size_t size, size_t &bytes_read)
{
    ENSURE_STATE_OR_RETURN(ReaderState::Data, RET_FATAL);

    int ret = m_format->read_data(*m_file, buf, size, bytes_read);
    if (ret == RET_OK) {
        // Do not alter state. Stay in ReaderState::DATA
    } else if (ret <= RET_FATAL) {
        m_state = ReaderState::Fatal;
    }

    return ret;
}

/*!
 * \brief Get detected or forced boot image format code.
 *
 * * If enable_format_*() was used, then the detected boot image format code is
 *   returned.
 * * If set_format_*() was used, then the forced boot image format code is
 *   returned.
 *
 * \note The return value is meaningful only after the boot image has been
 *       successfully opened. Otherwise, an error will be returned.
 *
 * \return Boot image format code or -1 if the boot image is not open
 */
int Reader::format_code()
{
    ENSURE_STATE_OR_RETURN(~ReaderStates(ReaderState::Moved), -1);

    if (!m_format) {
        set_error(ReaderError::NoFormatSelected);
        return -1;
    }

    return m_format->type();
}

/*!
 * \brief Get detected or forced boot image format name.
 *
 * * If enable_format_*() was used, then the detected boot image format name is
 *   returned.
 * * If set_format_*() was used, then the forced boot image format name is
 *   returned.
 *
 * \note The return value is meaningful only after the boot image has been
 *       successfully opened. Otherwise, an error will be returned.
 *
 * \return Boot image format name or empty string if the boot image is not open
 */
std::string Reader::format_name()
{
    ENSURE_STATE_OR_RETURN(~ReaderStates(ReaderState::Moved), {});

    if (!m_format) {
        set_error(ReaderError::NoFormatSelected);
        return {};
    }

    return m_format->name();
}

/*!
 * \brief Force support for a boot image format by its code.
 *
 * Calling this function causes the bidding process to be skipped. The chosen
 * format will be used regardless of which formats are enabled.
 *
 * \param code Boot image format code (\ref MB_BI_FORMAT_CODES)
 *
 * \return
 *   * #RET_OK if the format is successfully enabled
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::set_format_by_code(int code)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);
    FormatReader *format = nullptr;

    int ret = enable_format_by_code(code);
    if (ret < 0 && ret != RET_WARN) {
        return ret;
    }

    for (auto &f : m_formats) {
        if ((FORMAT_BASE_MASK & f->type() & code)
                == (FORMAT_BASE_MASK & code)) {
            format = f.get();
            break;
        }
    }

    if (!format) {
        set_error(ReaderError::EnabledFormatNotFound);
        m_state = ReaderState::Fatal;
        return RET_FATAL;
    }

    m_format = format;

    return RET_OK;
}

/*!
 * \brief Force support for a boot image format by its name.
 *
 * Calling this function causes the bidding process to be skipped. The chosen
 * format will be used regardless of which formats are enabled.
 *
 * \param name Boot image format name (\ref MB_BI_FORMAT_NAMES)
 *
 * \return
 *   * #RET_OK if the format is successfully set
 *   * \<= #RET_WARN if an error occurs
 */
int Reader::set_format_by_name(const std::string &name)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);
    FormatReader *format = nullptr;

    int ret = enable_format_by_name(name);
    if (ret < 0 && ret != RET_WARN) {
        return ret;
    }

    for (auto &f : m_formats) {
        if (f->name() == name) {
            format = f.get();
            break;
        }
    }

    if (!format) {
        set_error(ReaderError::EnabledFormatNotFound);
        m_state = ReaderState::Fatal;
        return RET_FATAL;
    }

    m_format = format;

    return RET_OK;
}

/*!
 * \brief Enable support for all boot image formats.
 *
 * \return
 *   * #RET_OK if all formats are successfully enabled
 *   * \<= #RET_FAILED if an error occurs while enabling a format
 */
int Reader::enable_format_all()
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);

    for (auto const &format : g_reader_formats) {
        int ret = (this->*format.func)();
        if (ret != RET_OK && ret != RET_WARN) {
            return ret;
        }
    }

    return RET_OK;
}

/*!
 * \brief Enable support for a boot image format by its code.
 *
 * \param code Boot image format code (\ref MB_BI_FORMAT_CODES)
 *
 * \return
 *   * #RET_OK if the format is successfully enabled
 *   * #RET_WARN if the format is already enabled
 *   * \<= #RET_FAILED if an error occurs
 */
int Reader::enable_format_by_code(int code)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);

    for (auto const &format : g_reader_formats) {
        if ((code & FORMAT_BASE_MASK) == (format.code & FORMAT_BASE_MASK)) {
            return (this->*format.func)();
        }
    }

    set_error(ReaderError::InvalidFormatCode,
              "Invalid format code: %d", code);
    return RET_FAILED;
}

/*!
 * \brief Enable support for a boot image format by its name.
 *
 * \param name Boot image format name (\ref MB_BI_FORMAT_NAMES)
 *
 * \return
 *   * #RET_OK if the format was successfully enabled
 *   * #RET_WARN if the format is already enabled
 *   * \<= #RET_FAILED if an error occurs
 */
int Reader::enable_format_by_name(const std::string &name)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FATAL);

    for (auto const &format : g_reader_formats) {
        if (name == format.name) {
            return (this->*format.func)();
        }
    }

    set_error(ReaderError::InvalidFormatName,
              "Invalid format name: %s", name.c_str());
    return RET_FAILED;
}

/*!
 * \brief Get error code for a failed operation.
 *
 * \note The return value is undefined if an operation did not fail.
 *
 * \return Error code for failed operation.
 */
std::error_code Reader::error()
{
    ENSURE_STATE_OR_RETURN(~ReaderStates(ReaderState::Moved), {});

    return m_error_code;
}

/*!
 * \brief Get error string for a failed operation.
 *
 * \note The return value is undefined if an operation did not fail.
 *
 * \return Error string for failed operation. The string contents may be
 *         undefined.
 */
std::string Reader::error_string()
{
    ENSURE_STATE_OR_RETURN(~ReaderStates(ReaderState::Moved), {});

    return m_error_string;
}

/*!
 * \brief Set error string for a failed operation.
 *
 * \sa Reader::set_error_v()
 *
 * \param ec Error code
 *
 * \return RET_OK if the error is successfully set or RET_FAILED if an
 *         error occurs
 */
int Reader::set_error(std::error_code ec)
{
    return set_error(ec, "%s", "");
}

/*!
 * \brief Set error string for a failed operation.
 *
 * \sa Reader::set_error_v()
 *
 * \param ec Error code
 * \param fmt `printf()`-style format string
 * \param ... `printf()`-style format arguments
 *
 * \return RET_OK if the error is successfully set or RET_FAILED if an
 *         error occurs
 */
int Reader::set_error(std::error_code ec, const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = set_error_v(ec, fmt, ap);
    va_end(ap);

    return ret;
}

/*!
 * \brief Set error string for a failed operation.
 *
 * \sa Reader::set_error()
 *
 * \param ec Error code
 * \param fmt `printf()`-style format string
 * \param ap `printf()`-style format arguments as a va_list
 *
 * \return RET_OK if the error is successfully set or RET_FAILED if an
 *         error occurs
 */
int Reader::set_error_v(std::error_code ec, const char *fmt, va_list ap)
{
    ENSURE_STATE_OR_RETURN(~ReaderStates(ReaderState::Moved), RET_FATAL);

    m_error_code = ec;

    auto result = format_v_safe(fmt, ap);
    if (!result) {
        return RET_FAILED;
    }
    m_error_string = std::move(result.value());

    if (!m_error_string.empty()) {
        m_error_string += ": ";
    }
    m_error_string += ec.message();

    return RET_OK;
}

int Reader::register_format(std::unique_ptr<FormatReader> format)
{
    ENSURE_STATE_OR_RETURN(ReaderState::New, RET_FAILED);

    for (auto const &f : m_formats) {
        if ((FORMAT_BASE_MASK & f->type())
                == (FORMAT_BASE_MASK & format->type())) {
            set_error(ReaderError::FormatAlreadyEnabled,
                      "%s format (0x%x) already enabled",
                      format->name().c_str(), format->type());
            return RET_WARN;
        }
    }

    m_formats.push_back(std::move(format));
    return RET_OK;
}

}
}
