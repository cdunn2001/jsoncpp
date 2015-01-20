// Copyright 2011 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
#include <json/assertions.h>
#include <json/value.h>
#include <json/writer.h>
#ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
#include "json_batchallocator.h"
#endif // #ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <math.h>
#include <sstream>
#include <utility>
#include <cstring>
#include <cassert>
#ifdef JSON_USE_CPPTL
#include <cpptl/conststring.h>
#endif
#include <cstddef> // size_t

#define JSON_ASSERT_UNREACHABLE assert(false)

namespace Json {

class JSON_API Value {
  friend class ValueIteratorBase;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
  friend class ValueInternalLink;
  friend class ValueInternalMap;
#endif
public:
  typedef std::vector<std::string> Members;
  typedef ValueIterator iterator;
  typedef ValueConstIterator const_iterator;
  typedef Json::UInt UInt;
  typedef Json::Int Int;
#if defined(JSON_HAS_INT64)
  typedef Json::UInt64 UInt64;
  typedef Json::Int64 Int64;
#endif // defined(JSON_HAS_INT64)
  typedef Json::LargestInt LargestInt;
  typedef Json::LargestUInt LargestUInt;
  typedef Json::ArrayIndex ArrayIndex;

  static const Value& null;
  /// Minimum signed integer value that can be stored in a Json::Value.
  static const LargestInt minLargestInt;
  /// Maximum signed integer value that can be stored in a Json::Value.
  static const LargestInt maxLargestInt;
  /// Maximum unsigned integer value that can be stored in a Json::Value.
  static const LargestUInt maxLargestUInt;

  /// Minimum signed int value that can be stored in a Json::Value.
  static const Int minInt;
  /// Maximum signed int value that can be stored in a Json::Value.
  static const Int maxInt;
  /// Maximum unsigned int value that can be stored in a Json::Value.
  static const UInt maxUInt;

#if defined(JSON_HAS_INT64)
  /// Minimum signed 64 bits int value that can be stored in a Json::Value.
  static const Int64 minInt64;
  /// Maximum signed 64 bits int value that can be stored in a Json::Value.
  static const Int64 maxInt64;
  /// Maximum unsigned 64 bits int value that can be stored in a Json::Value.
  static const UInt64 maxUInt64;
#endif // defined(JSON_HAS_INT64)

private:
#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  class CZString {
  public:
    enum DuplicationPolicy {
      noDuplication = 0,
      duplicate,
      duplicateOnCopy
    };
    CZString(ArrayIndex index);
    CZString(const char* cstr, DuplicationPolicy allocate);
    CZString(const CZString& other);
    ~CZString();
    CZString& operator=(CZString other);
    bool operator<(const CZString& other) const;
    bool operator==(const CZString& other) const;
    ArrayIndex index() const;
    const char* c_str() const;
    bool isStaticString() const;

  private:
    void swap(CZString& other);
    const char* cstr_;
    ArrayIndex index_;
  };

public:
#ifndef JSON_USE_CPPTL_SMALLMAP
  typedef std::map<CZString, Value> ObjectValues;
#else
  typedef CppTL::SmallMap<CZString, Value> ObjectValues;
#endif // ifndef JSON_USE_CPPTL_SMALLMAP
#endif // ifndef JSON_VALUE_USE_INTERNAL_MAP
#endif // ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

public:
  /** \brief Create a default Value of the given type.

    This is a very useful constructor.
    To create an empty array, pass arrayValue.
    To create an empty object, pass objectValue.
    Another Value can then be set to this one by assignment.
This is useful since clear() and resize() will not alter types.

    Examples:
\code
Json::Value null_value; // null
Json::Value arr_value(Json::arrayValue); // []
Json::Value obj_value(Json::objectValue); // {}
\endcode
  */
  Value(ValueType type = nullValue);
  Value(Int value);
  Value(UInt value);
#if defined(JSON_HAS_INT64)
  Value(Int64 value);
  Value(UInt64 value);
#endif // if defined(JSON_HAS_INT64)
  Value(double value);
  Value(const char* value);
  Value(const char* beginValue, const char* endValue);
  /** \brief Constructs a value from a static string.

   * Like other value string constructor but do not duplicate the string for
   * internal storage. The given string must remain alive after the call to this
   * constructor.
   * Example of usage:
   * \code
   * Json::Value aValue( StaticString("some text") );
   * \endcode
   */
  Value(const StaticString& value);
  Value(const std::string& value);
#ifdef JSON_USE_CPPTL
  Value(const CppTL::ConstString& value);
#endif
  Value(bool value);
  Value(const Value& other);
  ~Value();

  Value& operator=(Value other);
  /// Swap values.
  void swap(Value& other);

  ValueType type() const;

  bool operator<(const Value& other) const;
  bool operator<=(const Value& other) const;
  bool operator>=(const Value& other) const;
  bool operator>(const Value& other) const;

  bool operator==(const Value& other) const;
  bool operator!=(const Value& other) const;

  int compare(const Value& other) const;

  const char* asCString() const;
  std::string asString() const;
#ifdef JSON_USE_CPPTL
  CppTL::ConstString asConstString() const;
#endif
  Int asInt() const;
  UInt asUInt() const;
#if defined(JSON_HAS_INT64)
  Int64 asInt64() const;
  UInt64 asUInt64() const;
#endif // if defined(JSON_HAS_INT64)
  LargestInt asLargestInt() const;
  LargestUInt asLargestUInt() const;
  float asFloat() const;
  double asDouble() const;
  bool asBool() const;

  bool isNull() const;
  bool isBool() const;
  bool isInt() const;
  bool isInt64() const;
  bool isUInt() const;
  bool isUInt64() const;
  bool isIntegral() const;
  bool isDouble() const;
  bool isNumeric() const;
  bool isString() const;
  bool isArray() const;
  bool isObject() const;

  bool isConvertibleTo(ValueType other) const;

  /// Number of values in array or object
  ArrayIndex size() const;

  /// \brief Return true if empty array, empty object, or null;
  /// otherwise, false.
  bool empty() const;

  /// Return isNull()
  bool operator!() const;

  /// Remove all object members and array elements.
  /// \pre type() is arrayValue, objectValue, or nullValue
  /// \post type() is unchanged
  void clear();

  /// Resize the array to size elements.
  /// New elements are initialized to null.
  /// May only be called on nullValue or arrayValue.
  /// \pre type() is arrayValue or nullValue
  /// \post type() is arrayValue
  void resize(ArrayIndex size);

  /// Access an array element (zero based index ).
  /// If the array contains less than index element, then null value are
  /// inserted
  /// in the array so that its size is index+1.
  /// (You may need to say 'value[0u]' to get your compiler to distinguish
  ///  this from the operator[] which takes a string.)
  Value& operator[](ArrayIndex index);

  /// Access an array element (zero based index ).
  /// If the array contains less than index element, then null value are
  /// inserted
  /// in the array so that its size is index+1.
  /// (You may need to say 'value[0u]' to get your compiler to distinguish
  ///  this from the operator[] which takes a string.)
  Value& operator[](int index);

  /// Access an array element (zero based index )
  /// (You may need to say 'value[0u]' to get your compiler to distinguish
  ///  this from the operator[] which takes a string.)
  const Value& operator[](ArrayIndex index) const;

  /// Access an array element (zero based index )
  /// (You may need to say 'value[0u]' to get your compiler to distinguish
  ///  this from the operator[] which takes a string.)
  const Value& operator[](int index) const;

  /// If the array contains at least index+1 elements, returns the element
  /// value,
  /// otherwise returns defaultValue.
  Value get(ArrayIndex index, const Value& defaultValue) const;
  /// Return true if index < size().
  bool isValidIndex(ArrayIndex index) const;
  /// \brief Append value to array at the end.
  ///
  /// Equivalent to jsonvalue[jsonvalue.size()] = value;
  Value& append(const Value& value);

  /// Access an object value by name, create a null member if it does not exist.
  Value& operator[](const char* key);
  /// Access an object value by name, returns null if there is no member with
  /// that name.
  const Value& operator[](const char* key) const;
  /// Access an object value by name, create a null member if it does not exist.
  Value& operator[](const std::string& key);
  /// Access an object value by name, returns null if there is no member with
  /// that name.
  const Value& operator[](const std::string& key) const;
  /** \brief Access an object value by name, create a null member if it does not
   exist.

   * If the object as no entry for that name, then the member name used to store
   * the new entry is not duplicated.
   * Example of use:
   * \code
   * Json::Value object;
   * static const StaticString code("code");
   * object[code] = 1234;
   * \endcode
   */
  Value& operator[](const StaticString& key);
#ifdef JSON_USE_CPPTL
  /// Access an object value by name, create a null member if it does not exist.
  Value& operator[](const CppTL::ConstString& key);
  /// Access an object value by name, returns null if there is no member with
  /// that name.
  const Value& operator[](const CppTL::ConstString& key) const;
#endif
  /// Return the member named key if it exist, defaultValue otherwise.
  Value get(const char* key, const Value& defaultValue) const;
  /// Return the member named key if it exist, defaultValue otherwise.
  Value get(const std::string& key, const Value& defaultValue) const;
#ifdef JSON_USE_CPPTL
  /// Return the member named key if it exist, defaultValue otherwise.
  Value get(const CppTL::ConstString& key, const Value& defaultValue) const;
#endif
  /// \brief Remove and return the named member.
  ///
  /// Do nothing if it did not exist.
  /// \return the removed Value, or null.
  /// \pre type() is objectValue or nullValue
  /// \post type() is unchanged
  Value removeMember(const char* key);
  /// Same as removeMember(const char*)
  Value removeMember(const std::string& key);

  /// Return true if the object has a member named key.
  bool isMember(const char* key) const;
  /// Return true if the object has a member named key.
  bool isMember(const std::string& key) const;
#ifdef JSON_USE_CPPTL
  /// Return true if the object has a member named key.
  bool isMember(const CppTL::ConstString& key) const;
#endif

  /// \brief Return a list of the member names.
  ///
  /// If null, return an empty list.
  /// \pre type() is objectValue or nullValue
  /// \post if type() was nullValue, it remains nullValue
  Members getMemberNames() const;

  //# ifdef JSON_USE_CPPTL
  //      EnumMemberNames enumMemberNames() const;
  //      EnumValues enumValues() const;
  //# endif

  /// Comments must be //... or /* ... */
  void setComment(const char* comment, CommentPlacement placement);
  /// Comments must be //... or /* ... */
  void setComment(const std::string& comment, CommentPlacement placement);
  bool hasComment(CommentPlacement placement) const;
  /// Include delimiters and embedded newlines.
  std::string getComment(CommentPlacement placement) const;

  std::string toStyledString() const;

  const_iterator begin() const;
  const_iterator end() const;

  iterator begin();
  iterator end();

  // Accessors for the [start, limit) range of bytes within the JSON text from
  // which this value was parsed, if any.
  void setOffsetStart(size_t start);
  void setOffsetLimit(size_t limit);
  size_t getOffsetStart() const;
  size_t getOffsetLimit() const;

private:
  void initBasic(ValueType type, bool allocated = false);

  Value& resolveReference(const char* key, bool isStatic);

  /// Swap values but leave comments and source offsets in place.
  void swapPayload(Value& other);

#ifdef JSON_VALUE_USE_INTERNAL_MAP
  inline bool isItemAvailable() const { return itemIsUsed_ == 0; }

  inline void setItemUsed(bool isUsed = true) { itemIsUsed_ = isUsed ? 1 : 0; }

  inline bool isMemberNameStatic() const { return memberNameIsStatic_ == 0; }

  inline void setMemberNameIsStatic(bool isStatic) {
    memberNameIsStatic_ = isStatic ? 1 : 0;
  }
#endif // # ifdef JSON_VALUE_USE_INTERNAL_MAP

private:
  struct CommentInfo {
    CommentInfo();
    ~CommentInfo();

    void setComment(const char* text);

    char* comment_;
  };

  // struct MemberNamesTransform
  //{
  //   typedef const char *result_type;
  //   const char *operator()( const CZString &name ) const
  //   {
  //      return name.c_str();
  //   }
  //};

  union ValueHolder {
    LargestInt int_;
    LargestUInt uint_;
    double real_;
    bool bool_;
    char* string_;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
    ValueInternalArray* array_;
    ValueInternalMap* map_;
#else
    ObjectValues* map_;
#endif
  } value_;
  ValueType type_ : 8;
  int allocated_ : 1; // Notes: if declared as bool, bitfield is useless.
#ifdef JSON_VALUE_USE_INTERNAL_MAP
  unsigned int itemIsUsed_ : 1; // used by the ValueInternalMap container.
  int memberNameIsStatic_ : 1;  // used by the ValueInternalMap container.
#endif
  CommentInfo* comments_;

  // [start, limit) byte offsets in the source JSON text from which this Value
  // was extracted.
  size_t start_;
  size_t limit_;
};


// This is a walkaround to avoid the static initialization of Value::null.
// kNull must be word-aligned to avoid crashing on ARM.  We use an alignment of
// 8 (instead of 4) as a bit of future-proofing.
#if defined(__ARMEL__)
#define ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#else
#define ALIGNAS(byte_alignment)
#endif
static const unsigned char ALIGNAS(8) kNull[sizeof(Value)] = { 0 };
const unsigned char& kNullRef = kNull[0];
const Value& Value::null = reinterpret_cast<const Value&>(kNullRef);

const Int Value::minInt = Int(~(UInt(-1) / 2));
const Int Value::maxInt = Int(UInt(-1) / 2);
const UInt Value::maxUInt = UInt(-1);
#if defined(JSON_HAS_INT64)
const Int64 Value::minInt64 = Int64(~(UInt64(-1) / 2));
const Int64 Value::maxInt64 = Int64(UInt64(-1) / 2);
const UInt64 Value::maxUInt64 = UInt64(-1);
// The constant is hard-coded because some compiler have trouble
// converting Value::maxUInt64 to a double correctly (AIX/xlC).
// Assumes that UInt64 is a 64 bits integer.
static const double maxUInt64AsDouble = 18446744073709551615.0;
#endif // defined(JSON_HAS_INT64)
const LargestInt Value::minLargestInt = LargestInt(~(LargestUInt(-1) / 2));
const LargestInt Value::maxLargestInt = LargestInt(LargestUInt(-1) / 2);
const LargestUInt Value::maxLargestUInt = LargestUInt(-1);

/// Unknown size marker
static const unsigned int unknown = (unsigned)-1;

#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
  return d >= min && d <= max;
}
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
static inline double integerToDouble(Json::UInt64 value) {
  return static_cast<double>(Int64(value / 2)) * 2.0 + Int64(value & 1);
}

template <typename T> static inline double integerToDouble(T value) {
  return static_cast<double>(value);
}

template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
  return d >= integerToDouble(min) && d <= integerToDouble(max);
}
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)

/** Duplicates the specified string value.
 * @param value Pointer to the string to duplicate. Must be zero-terminated if
 *              length is "unknown".
 * @param length Length of the value. if equals to unknown, then it will be
 *               computed using strlen(value).
 * @return Pointer on the duplicate instance of string.
 */
static inline char* duplicateStringValue(const char* value,
                                         unsigned int length = unknown) {
  if (length == unknown)
    length = (unsigned int)strlen(value);

  // Avoid an integer overflow in the call to malloc below by limiting length
  // to a sane value.
  if (length >= (unsigned)Value::maxInt)
    length = Value::maxInt - 1;

  char* newString = static_cast<char*>(malloc(length + 1));
  JSON_ASSERT_MESSAGE(newString != 0,
                      "in Json::Value::duplicateStringValue(): "
                      "Failed to allocate string value buffer");
  memcpy(newString, value, length);
  newString[length] = 0;
  return newString;
}

/** Free the string duplicated by duplicateStringValue().
 */
static inline void releaseStringValue(char* value) { free(value); }

} // namespace Json

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// ValueInternals...
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
#if !defined(JSON_IS_AMALGAMATION)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
#include "json_internalarray.inl"
#include "json_internalmap.inl"
#endif // JSON_VALUE_USE_INTERNAL_MAP

#include "json_valueiterator.inl"
#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::CommentInfo
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

Value::CommentInfo::CommentInfo() : comment_(0) {}

Value::CommentInfo::~CommentInfo() {
  if (comment_)
    releaseStringValue(comment_);
}

void Value::CommentInfo::setComment(const char* text) {
  if (comment_)
    releaseStringValue(comment_);
  JSON_ASSERT(text != 0);
  JSON_ASSERT_MESSAGE(
      text[0] == '\0' || text[0] == '/',
      "in Json::Value::setComment(): Comments must start with /");
  // It seems that /**/ style comments are acceptable as well.
  comment_ = duplicateStringValue(text);
}

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::CZString
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
#ifndef JSON_VALUE_USE_INTERNAL_MAP

// Notes: index_ indicates if the string was allocated when
// a string is stored.

Value::CZString::CZString(ArrayIndex index) : cstr_(0), index_(index) {}

Value::CZString::CZString(const char* cstr, DuplicationPolicy allocate)
    : cstr_(allocate == duplicate ? duplicateStringValue(cstr) : cstr),
      index_(allocate) {}

Value::CZString::CZString(const CZString& other)
    : cstr_(other.index_ != noDuplication && other.cstr_ != 0
                ? duplicateStringValue(other.cstr_)
                : other.cstr_),
      index_(other.cstr_
                 ? static_cast<ArrayIndex>(other.index_ == noDuplication
                     ? noDuplication : duplicate)
                 : other.index_) {}

Value::CZString::~CZString() {
  if (cstr_ && index_ == duplicate)
    releaseStringValue(const_cast<char*>(cstr_));
}

void Value::CZString::swap(CZString& other) {
  std::swap(cstr_, other.cstr_);
  std::swap(index_, other.index_);
}

Value::CZString& Value::CZString::operator=(CZString other) {
  swap(other);
  return *this;
}

bool Value::CZString::operator<(const CZString& other) const {
  if (cstr_)
    return strcmp(cstr_, other.cstr_) < 0;
  return index_ < other.index_;
}

bool Value::CZString::operator==(const CZString& other) const {
  if (cstr_)
    return strcmp(cstr_, other.cstr_) == 0;
  return index_ == other.index_;
}

ArrayIndex Value::CZString::index() const { return index_; }

const char* Value::CZString::c_str() const { return cstr_; }

bool Value::CZString::isStaticString() const { return index_ == noDuplication; }

#endif // ifndef JSON_VALUE_USE_INTERNAL_MAP

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::Value
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/*! \internal Default constructor initialization must be equivalent to:
 * memset( this, 0, sizeof(Value) )
 * This optimization is used in ValueInternalMap fast allocator.
 */
Value::Value(ValueType type) {
  initBasic(type);
  switch (type) {
  case nullValue:
    break;
  case intValue:
  case uintValue:
    value_.int_ = 0;
    break;
  case realValue:
    value_.real_ = 0.0;
    break;
  case stringValue:
    value_.string_ = 0;
    break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
  case objectValue:
    value_.map_ = new ObjectValues();
    break;
#else
  case arrayValue:
    value_.array_ = arrayAllocator()->newArray();
    break;
  case objectValue:
    value_.map_ = mapAllocator()->newMap();
    break;
#endif
  case booleanValue:
    value_.bool_ = false;
    break;
  default:
    JSON_ASSERT_UNREACHABLE;
  }
}

Value::Value(Int value) {
  initBasic(intValue);
  value_.int_ = value;
}

Value::Value(UInt value) {
  initBasic(uintValue);
  value_.uint_ = value;
}
#if defined(JSON_HAS_INT64)
Value::Value(Int64 value) {
  initBasic(intValue);
  value_.int_ = value;
}
Value::Value(UInt64 value) {
  initBasic(uintValue);
  value_.uint_ = value;
}
#endif // defined(JSON_HAS_INT64)

Value::Value(double value) {
  initBasic(realValue);
  value_.real_ = value;
}

Value::Value(const char* value) {
  initBasic(stringValue, true);
  value_.string_ = duplicateStringValue(value);
}

Value::Value(const char* beginValue, const char* endValue) {
  initBasic(stringValue, true);
  value_.string_ =
      duplicateStringValue(beginValue, (unsigned int)(endValue - beginValue));
}

Value::Value(const std::string& value) {
  initBasic(stringValue, true);
  value_.string_ =
      duplicateStringValue(value.c_str(), (unsigned int)value.length());
}

Value::Value(const StaticString& value) {
  initBasic(stringValue);
  value_.string_ = const_cast<char*>(value.c_str());
}

#ifdef JSON_USE_CPPTL
Value::Value(const CppTL::ConstString& value) {
  initBasic(stringValue, true);
  value_.string_ = duplicateStringValue(value, value.length());
}
#endif

Value::Value(bool value) {
  initBasic(booleanValue);
  value_.bool_ = value;
}

Value::Value(const Value& other)
    : type_(other.type_), allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0), start_(other.start_), limit_(other.limit_) {
  switch (type_) {
  case nullValue:
  case intValue:
  case uintValue:
  case realValue:
  case booleanValue:
    value_ = other.value_;
    break;
  case stringValue:
    if (other.value_.string_) {
      value_.string_ = duplicateStringValue(other.value_.string_);
      allocated_ = true;
    } else {
      value_.string_ = 0;
      allocated_ = false;
    }
    break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
  case objectValue:
    value_.map_ = new ObjectValues(*other.value_.map_);
    break;
#else
  case arrayValue:
    value_.array_ = arrayAllocator()->newArrayCopy(*other.value_.array_);
    break;
  case objectValue:
    value_.map_ = mapAllocator()->newMapCopy(*other.value_.map_);
    break;
#endif
  default:
    JSON_ASSERT_UNREACHABLE;
  }
  if (other.comments_) {
    comments_ = new CommentInfo[numberOfCommentPlacement];
    for (int comment = 0; comment < numberOfCommentPlacement; ++comment) {
      const CommentInfo& otherComment = other.comments_[comment];
      if (otherComment.comment_)
        comments_[comment].setComment(otherComment.comment_);
    }
  }
}

Value::~Value() {
  switch (type_) {
  case nullValue:
  case intValue:
  case uintValue:
  case realValue:
  case booleanValue:
    break;
  case stringValue:
    if (allocated_)
      releaseStringValue(value_.string_);
    break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
  case objectValue:
    delete value_.map_;
    break;
#else
  case arrayValue:
    arrayAllocator()->destructArray(value_.array_);
    break;
  case objectValue:
    mapAllocator()->destructMap(value_.map_);
    break;
#endif
  default:
    JSON_ASSERT_UNREACHABLE;
  }

  if (comments_)
    delete[] comments_;
}

Value& Value::operator=(Value other) {
  swapPayload(other);
  return *this;
}

void Value::swapPayload(Value& other) {
  ValueType temp = type_;
  type_ = other.type_;
  other.type_ = temp;
  std::swap(value_, other.value_);
  int temp2 = allocated_;
  allocated_ = other.allocated_;
  other.allocated_ = temp2;
}

void Value::swap(Value& other) {
  swapPayload(other);
  std::swap(comments_, other.comments_);
  std::swap(start_, other.start_);
  std::swap(limit_, other.limit_);
}

ValueType Value::type() const { return type_; }

int Value::compare(const Value& other) const {
  if (*this < other)
    return -1;
  if (*this > other)
    return 1;
  return 0;
}

bool Value::operator<(const Value& other) const {
  int typeDelta = type_ - other.type_;
  if (typeDelta)
    return typeDelta < 0 ? true : false;
  switch (type_) {
  case nullValue:
    return false;
  case intValue:
    return value_.int_ < other.value_.int_;
  case uintValue:
    return value_.uint_ < other.value_.uint_;
  case realValue:
    return value_.real_ < other.value_.real_;
  case booleanValue:
    return value_.bool_ < other.value_.bool_;
  case stringValue:
    return (value_.string_ == 0 && other.value_.string_) ||
           (other.value_.string_ && value_.string_ &&
            strcmp(value_.string_, other.value_.string_) < 0);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
  case objectValue: {
    int delta = int(value_.map_->size() - other.value_.map_->size());
    if (delta)
      return delta < 0;
    return (*value_.map_) < (*other.value_.map_);
  }
#else
  case arrayValue:
    return value_.array_->compare(*(other.value_.array_)) < 0;
  case objectValue:
    return value_.map_->compare(*(other.value_.map_)) < 0;
#endif
  default:
    JSON_ASSERT_UNREACHABLE;
  }
  return false; // unreachable
}

bool Value::operator<=(const Value& other) const { return !(other < *this); }

bool Value::operator>=(const Value& other) const { return !(*this < other); }

bool Value::operator>(const Value& other) const { return other < *this; }

bool Value::operator==(const Value& other) const {
  // if ( type_ != other.type_ )
  // GCC 2.95.3 says:
  // attempt to take address of bit-field structure member `Json::Value::type_'
  // Beats me, but a temp solves the problem.
  int temp = other.type_;
  if (type_ != temp)
    return false;
  switch (type_) {
  case nullValue:
    return true;
  case intValue:
    return value_.int_ == other.value_.int_;
  case uintValue:
    return value_.uint_ == other.value_.uint_;
  case realValue:
    return value_.real_ == other.value_.real_;
  case booleanValue:
    return value_.bool_ == other.value_.bool_;
  case stringValue:
    return (value_.string_ == other.value_.string_) ||
           (other.value_.string_ && value_.string_ &&
            strcmp(value_.string_, other.value_.string_) == 0);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
  case objectValue:
    return value_.map_->size() == other.value_.map_->size() &&
           (*value_.map_) == (*other.value_.map_);
#else
  case arrayValue:
    return value_.array_->compare(*(other.value_.array_)) == 0;
  case objectValue:
    return value_.map_->compare(*(other.value_.map_)) == 0;
#endif
  default:
    JSON_ASSERT_UNREACHABLE;
  }
  return false; // unreachable
}

bool Value::operator!=(const Value& other) const { return !(*this == other); }

const char* Value::asCString() const {
  JSON_ASSERT_MESSAGE(type_ == stringValue,
                      "in Json::Value::asCString(): requires stringValue");
  return value_.string_;
}

std::string Value::asString() const {
  switch (type_) {
  case nullValue:
    return "";
  case stringValue:
    return value_.string_ ? value_.string_ : "";
  case booleanValue:
    return value_.bool_ ? "true" : "false";
  case intValue:
    return valueToString(value_.int_);
  case uintValue:
    return valueToString(value_.uint_);
  case realValue:
    return valueToString(value_.real_);
  default:
    JSON_FAIL_MESSAGE("Type is not convertible to string");
  }
}

#ifdef JSON_USE_CPPTL
CppTL::ConstString Value::asConstString() const {
  return CppTL::ConstString(asString().c_str());
}
#endif

Value::Int Value::asInt() const {
  switch (type_) {
  case intValue:
    JSON_ASSERT_MESSAGE(isInt(), "LargestInt out of Int range");
    return Int(value_.int_);
  case uintValue:
    JSON_ASSERT_MESSAGE(isInt(), "LargestUInt out of Int range");
    return Int(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt, maxInt),
                        "double out of Int range");
    return Int(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to Int.");
}

Value::UInt Value::asUInt() const {
  switch (type_) {
  case intValue:
    JSON_ASSERT_MESSAGE(isUInt(), "LargestInt out of UInt range");
    return UInt(value_.int_);
  case uintValue:
    JSON_ASSERT_MESSAGE(isUInt(), "LargestUInt out of UInt range");
    return UInt(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt),
                        "double out of UInt range");
    return UInt(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to UInt.");
}

#if defined(JSON_HAS_INT64)

Value::Int64 Value::asInt64() const {
  switch (type_) {
  case intValue:
    return Int64(value_.int_);
  case uintValue:
    JSON_ASSERT_MESSAGE(isInt64(), "LargestUInt out of Int64 range");
    return Int64(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt64, maxInt64),
                        "double out of Int64 range");
    return Int64(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to Int64.");
}

Value::UInt64 Value::asUInt64() const {
  switch (type_) {
  case intValue:
    JSON_ASSERT_MESSAGE(isUInt64(), "LargestInt out of UInt64 range");
    return UInt64(value_.int_);
  case uintValue:
    return UInt64(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt64),
                        "double out of UInt64 range");
    return UInt64(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to UInt64.");
}
#endif // if defined(JSON_HAS_INT64)

LargestInt Value::asLargestInt() const {
#if defined(JSON_NO_INT64)
  return asInt();
#else
  return asInt64();
#endif
}

LargestUInt Value::asLargestUInt() const {
#if defined(JSON_NO_INT64)
  return asUInt();
#else
  return asUInt64();
#endif
}

double Value::asDouble() const {
  switch (type_) {
  case intValue:
    return static_cast<double>(value_.int_);
  case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    return static_cast<double>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    return integerToDouble(value_.uint_);
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
  case realValue:
    return value_.real_;
  case nullValue:
    return 0.0;
  case booleanValue:
    return value_.bool_ ? 1.0 : 0.0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to double.");
}

float Value::asFloat() const {
  switch (type_) {
  case intValue:
    return static_cast<float>(value_.int_);
  case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    return static_cast<float>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    return integerToDouble(value_.uint_);
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
  case realValue:
    return static_cast<float>(value_.real_);
  case nullValue:
    return 0.0;
  case booleanValue:
    return value_.bool_ ? 1.0f : 0.0f;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to float.");
}

bool Value::asBool() const {
  switch (type_) {
  case booleanValue:
    return value_.bool_;
  case nullValue:
    return false;
  case intValue:
    return value_.int_ ? true : false;
  case uintValue:
    return value_.uint_ ? true : false;
  case realValue:
    return value_.real_ ? true : false;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to bool.");
}

bool Value::isConvertibleTo(ValueType other) const {
  switch (other) {
  case nullValue:
    return (isNumeric() && asDouble() == 0.0) ||
           (type_ == booleanValue && value_.bool_ == false) ||
           (type_ == stringValue && asString() == "") ||
           (type_ == arrayValue && value_.map_->size() == 0) ||
           (type_ == objectValue && value_.map_->size() == 0) ||
           type_ == nullValue;
  case intValue:
    return isInt() ||
           (type_ == realValue && InRange(value_.real_, minInt, maxInt)) ||
           type_ == booleanValue || type_ == nullValue;
  case uintValue:
    return isUInt() ||
           (type_ == realValue && InRange(value_.real_, 0, maxUInt)) ||
           type_ == booleanValue || type_ == nullValue;
  case realValue:
    return isNumeric() || type_ == booleanValue || type_ == nullValue;
  case booleanValue:
    return isNumeric() || type_ == booleanValue || type_ == nullValue;
  case stringValue:
    return isNumeric() || type_ == booleanValue || type_ == stringValue ||
           type_ == nullValue;
  case arrayValue:
    return type_ == arrayValue || type_ == nullValue;
  case objectValue:
    return type_ == objectValue || type_ == nullValue;
  }
  JSON_ASSERT_UNREACHABLE;
  return false;
}

/// Number of values in array or object
ArrayIndex Value::size() const {
  switch (type_) {
  case nullValue:
  case intValue:
  case uintValue:
  case realValue:
  case booleanValue:
  case stringValue:
    return 0;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue: // size of the array is highest index + 1
    if (!value_.map_->empty()) {
      ObjectValues::const_iterator itLast = value_.map_->end();
      --itLast;
      return (*itLast).first.index() + 1;
    }
    return 0;
  case objectValue:
    return ArrayIndex(value_.map_->size());
#else
  case arrayValue:
    return Int(value_.array_->size());
  case objectValue:
    return Int(value_.map_->size());
#endif
  }
  JSON_ASSERT_UNREACHABLE;
  return 0; // unreachable;
}

bool Value::empty() const {
  if (isNull() || isArray() || isObject())
    return size() == 0u;
  else
    return false;
}

bool Value::operator!() const { return isNull(); }

void Value::clear() {
  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue ||
                          type_ == objectValue,
                      "in Json::Value::clear(): requires complex value");
  start_ = 0;
  limit_ = 0;
  switch (type_) {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
  case objectValue:
    value_.map_->clear();
    break;
#else
  case arrayValue:
    value_.array_->clear();
    break;
  case objectValue:
    value_.map_->clear();
    break;
#endif
  default:
    break;
  }
}

void Value::resize(ArrayIndex newSize) {
  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue,
                      "in Json::Value::resize(): requires arrayValue");
  if (type_ == nullValue)
    *this = Value(arrayValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  ArrayIndex oldSize = size();
  if (newSize == 0)
    clear();
  else if (newSize > oldSize)
    (*this)[newSize - 1];
  else {
    for (ArrayIndex index = newSize; index < oldSize; ++index) {
      value_.map_->erase(index);
    }
    assert(size() == newSize);
  }
#else
  value_.array_->resize(newSize);
#endif
}

Value& Value::operator[](ArrayIndex index) {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == arrayValue,
      "in Json::Value::operator[](ArrayIndex): requires arrayValue");
  if (type_ == nullValue)
    *this = Value(arrayValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  CZString key(index);
  ObjectValues::iterator it = value_.map_->lower_bound(key);
  if (it != value_.map_->end() && (*it).first == key)
    return (*it).second;

  ObjectValues::value_type defaultValue(key, null);
  it = value_.map_->insert(it, defaultValue);
  return (*it).second;
#else
  return value_.array_->resolveReference(index);
#endif
}

Value& Value::operator[](int index) {
  JSON_ASSERT_MESSAGE(
      index >= 0,
      "in Json::Value::operator[](int index): index cannot be negative");
  return (*this)[ArrayIndex(index)];
}

const Value& Value::operator[](ArrayIndex index) const {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == arrayValue,
      "in Json::Value::operator[](ArrayIndex)const: requires arrayValue");
  if (type_ == nullValue)
    return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  CZString key(index);
  ObjectValues::const_iterator it = value_.map_->find(key);
  if (it == value_.map_->end())
    return null;
  return (*it).second;
#else
  Value* value = value_.array_->find(index);
  return value ? *value : null;
#endif
}

const Value& Value::operator[](int index) const {
  JSON_ASSERT_MESSAGE(
      index >= 0,
      "in Json::Value::operator[](int index) const: index cannot be negative");
  return (*this)[ArrayIndex(index)];
}

Value& Value::operator[](const char* key) {
  return resolveReference(key, false);
}

void Value::initBasic(ValueType type, bool allocated) {
  type_ = type;
  allocated_ = allocated;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
  itemIsUsed_ = 0;
#endif
  comments_ = 0;
  start_ = 0;
  limit_ = 0;
}

Value& Value::resolveReference(const char* key, bool isStatic) {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == objectValue,
      "in Json::Value::resolveReference(): requires objectValue");
  if (type_ == nullValue)
    *this = Value(objectValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  CZString actualKey(
      key, isStatic ? CZString::noDuplication : CZString::duplicateOnCopy);
  ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
  if (it != value_.map_->end() && (*it).first == actualKey)
    return (*it).second;

  ObjectValues::value_type defaultValue(actualKey, null);
  it = value_.map_->insert(it, defaultValue);
  Value& value = (*it).second;
  return value;
#else
  return value_.map_->resolveReference(key, isStatic);
#endif
}

Value Value::get(ArrayIndex index, const Value& defaultValue) const {
  const Value* value = &((*this)[index]);
  return value == &null ? defaultValue : *value;
}

bool Value::isValidIndex(ArrayIndex index) const { return index < size(); }

const Value& Value::operator[](const char* key) const {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == objectValue,
      "in Json::Value::operator[](char const*)const: requires objectValue");
  if (type_ == nullValue)
    return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  CZString actualKey(key, CZString::noDuplication);
  ObjectValues::const_iterator it = value_.map_->find(actualKey);
  if (it == value_.map_->end())
    return null;
  return (*it).second;
#else
  const Value* value = value_.map_->find(key);
  return value ? *value : null;
#endif
}

Value& Value::operator[](const std::string& key) {
  return (*this)[key.c_str()];
}

const Value& Value::operator[](const std::string& key) const {
  return (*this)[key.c_str()];
}

Value& Value::operator[](const StaticString& key) {
  return resolveReference(key, true);
}

#ifdef JSON_USE_CPPTL
Value& Value::operator[](const CppTL::ConstString& key) {
  return (*this)[key.c_str()];
}

const Value& Value::operator[](const CppTL::ConstString& key) const {
  return (*this)[key.c_str()];
}
#endif

Value& Value::append(const Value& value) { return (*this)[size()] = value; }

Value Value::get(const char* key, const Value& defaultValue) const {
  const Value* value = &((*this)[key]);
  return value == &null ? defaultValue : *value;
}

Value Value::get(const std::string& key, const Value& defaultValue) const {
  return get(key.c_str(), defaultValue);
}

Value Value::removeMember(const char* key) {
  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == objectValue,
                      "in Json::Value::removeMember(): requires objectValue");
  if (type_ == nullValue)
    return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  CZString actualKey(key, CZString::noDuplication);
  ObjectValues::iterator it = value_.map_->find(actualKey);
  if (it == value_.map_->end())
    return null;
  Value old(it->second);
  value_.map_->erase(it);
  return old;
#else
  Value* value = value_.map_->find(key);
  if (value) {
    Value old(*value);
    value_.map_.remove(key);
    return old;
  } else {
    return null;
  }
#endif
}

Value Value::removeMember(const std::string& key) {
  return removeMember(key.c_str());
}

#ifdef JSON_USE_CPPTL
Value Value::get(const CppTL::ConstString& key,
                 const Value& defaultValue) const {
  return get(key.c_str(), defaultValue);
}
#endif

bool Value::isMember(const char* key) const {
  const Value* value = &((*this)[key]);
  return value != &null;
}

bool Value::isMember(const std::string& key) const {
  return isMember(key.c_str());
}

#ifdef JSON_USE_CPPTL
bool Value::isMember(const CppTL::ConstString& key) const {
  return isMember(key.c_str());
}
#endif

Value::Members Value::getMemberNames() const {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == objectValue,
      "in Json::Value::getMemberNames(), value must be objectValue");
  if (type_ == nullValue)
    return Value::Members();
  Members members;
  members.reserve(value_.map_->size());
#ifndef JSON_VALUE_USE_INTERNAL_MAP
  ObjectValues::const_iterator it = value_.map_->begin();
  ObjectValues::const_iterator itEnd = value_.map_->end();
  for (; it != itEnd; ++it)
    members.push_back(std::string((*it).first.c_str()));
#else
  ValueInternalMap::IteratorState it;
  ValueInternalMap::IteratorState itEnd;
  value_.map_->makeBeginIterator(it);
  value_.map_->makeEndIterator(itEnd);
  for (; !ValueInternalMap::equals(it, itEnd); ValueInternalMap::increment(it))
    members.push_back(std::string(ValueInternalMap::key(it)));
#endif
  return members;
}
//
//# ifdef JSON_USE_CPPTL
// EnumMemberNames
// Value::enumMemberNames() const
//{
//   if ( type_ == objectValue )
//   {
//      return CppTL::Enum::any(  CppTL::Enum::transform(
//         CppTL::Enum::keys( *(value_.map_), CppTL::Type<const CZString &>() ),
//         MemberNamesTransform() ) );
//   }
//   return EnumMemberNames();
//}
//
//
// EnumValues
// Value::enumValues() const
//{
//   if ( type_ == objectValue  ||  type_ == arrayValue )
//      return CppTL::Enum::anyValues( *(value_.map_),
//                                     CppTL::Type<const Value &>() );
//   return EnumValues();
//}
//
//# endif

static bool IsIntegral(double d) {
  double integral_part;
  return modf(d, &integral_part) == 0.0;
}

bool Value::isNull() const { return type_ == nullValue; }

bool Value::isBool() const { return type_ == booleanValue; }

bool Value::isInt() const {
  switch (type_) {
  case intValue:
    return value_.int_ >= minInt && value_.int_ <= maxInt;
  case uintValue:
    return value_.uint_ <= UInt(maxInt);
  case realValue:
    return value_.real_ >= minInt && value_.real_ <= maxInt &&
           IsIntegral(value_.real_);
  default:
    break;
  }
  return false;
}

bool Value::isUInt() const {
  switch (type_) {
  case intValue:
    return value_.int_ >= 0 && LargestUInt(value_.int_) <= LargestUInt(maxUInt);
  case uintValue:
    return value_.uint_ <= maxUInt;
  case realValue:
    return value_.real_ >= 0 && value_.real_ <= maxUInt &&
           IsIntegral(value_.real_);
  default:
    break;
  }
  return false;
}

bool Value::isInt64() const {
#if defined(JSON_HAS_INT64)
  switch (type_) {
  case intValue:
    return true;
  case uintValue:
    return value_.uint_ <= UInt64(maxInt64);
  case realValue:
    // Note that maxInt64 (= 2^63 - 1) is not exactly representable as a
    // double, so double(maxInt64) will be rounded up to 2^63. Therefore we
    // require the value to be strictly less than the limit.
    return value_.real_ >= double(minInt64) &&
           value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
  default:
    break;
  }
#endif // JSON_HAS_INT64
  return false;
}

bool Value::isUInt64() const {
#if defined(JSON_HAS_INT64)
  switch (type_) {
  case intValue:
    return value_.int_ >= 0;
  case uintValue:
    return true;
  case realValue:
    // Note that maxUInt64 (= 2^64 - 1) is not exactly representable as a
    // double, so double(maxUInt64) will be rounded up to 2^64. Therefore we
    // require the value to be strictly less than the limit.
    return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble &&
           IsIntegral(value_.real_);
  default:
    break;
  }
#endif // JSON_HAS_INT64
  return false;
}

bool Value::isIntegral() const {
#if defined(JSON_HAS_INT64)
  return isInt64() || isUInt64();
#else
  return isInt() || isUInt();
#endif
}

bool Value::isDouble() const { return type_ == realValue || isIntegral(); }

bool Value::isNumeric() const { return isIntegral() || isDouble(); }

bool Value::isString() const { return type_ == stringValue; }

bool Value::isArray() const { return type_ == arrayValue; }

bool Value::isObject() const { return type_ == objectValue; }

void Value::setComment(const char* comment, CommentPlacement placement) {
  if (!comments_)
    comments_ = new CommentInfo[numberOfCommentPlacement];
  comments_[placement].setComment(comment);
}

void Value::setComment(const std::string& comment, CommentPlacement placement) {
  setComment(comment.c_str(), placement);
}

bool Value::hasComment(CommentPlacement placement) const {
  return comments_ != 0 && comments_[placement].comment_ != 0;
}

std::string Value::getComment(CommentPlacement placement) const {
  if (hasComment(placement))
    return comments_[placement].comment_;
  return "";
}

void Value::setOffsetStart(size_t start) { start_ = start; }

void Value::setOffsetLimit(size_t limit) { limit_ = limit; }

size_t Value::getOffsetStart() const { return start_; }

size_t Value::getOffsetLimit() const { return limit_; }

std::string Value::toStyledString() const {
  StyledWriter writer;
  return writer.write(*this);
}

Value::const_iterator Value::begin() const {
  switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
    if (value_.array_) {
      ValueInternalArray::IteratorState it;
      value_.array_->makeBeginIterator(it);
      return const_iterator(it);
    }
    break;
  case objectValue:
    if (value_.map_) {
      ValueInternalMap::IteratorState it;
      value_.map_->makeBeginIterator(it);
      return const_iterator(it);
    }
    break;
#else
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return const_iterator(value_.map_->begin());
    break;
#endif
  default:
    break;
  }
  return const_iterator();
}

Value::const_iterator Value::end() const {
  switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
    if (value_.array_) {
      ValueInternalArray::IteratorState it;
      value_.array_->makeEndIterator(it);
      return const_iterator(it);
    }
    break;
  case objectValue:
    if (value_.map_) {
      ValueInternalMap::IteratorState it;
      value_.map_->makeEndIterator(it);
      return const_iterator(it);
    }
    break;
#else
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return const_iterator(value_.map_->end());
    break;
#endif
  default:
    break;
  }
  return const_iterator();
}

Value::iterator Value::begin() {
  switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
    if (value_.array_) {
      ValueInternalArray::IteratorState it;
      value_.array_->makeBeginIterator(it);
      return iterator(it);
    }
    break;
  case objectValue:
    if (value_.map_) {
      ValueInternalMap::IteratorState it;
      value_.map_->makeBeginIterator(it);
      return iterator(it);
    }
    break;
#else
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return iterator(value_.map_->begin());
    break;
#endif
  default:
    break;
  }
  return iterator();
}

Value::iterator Value::end() {
  switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
  case arrayValue:
    if (value_.array_) {
      ValueInternalArray::IteratorState it;
      value_.array_->makeEndIterator(it);
      return iterator(it);
    }
    break;
  case objectValue:
    if (value_.map_) {
      ValueInternalMap::IteratorState it;
      value_.map_->makeEndIterator(it);
      return iterator(it);
    }
    break;
#else
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return iterator(value_.map_->end());
    break;
#endif
  default:
    break;
  }
  return iterator();
}

// class PathArgument
// //////////////////////////////////////////////////////////////////

PathArgument::PathArgument() : key_(), index_(), kind_(kindNone) {}

PathArgument::PathArgument(ArrayIndex index)
    : key_(), index_(index), kind_(kindIndex) {}

PathArgument::PathArgument(const char* key)
    : key_(key), index_(), kind_(kindKey) {}

PathArgument::PathArgument(const std::string& key)
    : key_(key.c_str()), index_(), kind_(kindKey) {}

// class Path
// //////////////////////////////////////////////////////////////////

Path::Path(const std::string& path,
           const PathArgument& a1,
           const PathArgument& a2,
           const PathArgument& a3,
           const PathArgument& a4,
           const PathArgument& a5) {
  InArgs in;
  in.push_back(&a1);
  in.push_back(&a2);
  in.push_back(&a3);
  in.push_back(&a4);
  in.push_back(&a5);
  makePath(path, in);
}

void Path::makePath(const std::string& path, const InArgs& in) {
  const char* current = path.c_str();
  const char* end = current + path.length();
  InArgs::const_iterator itInArg = in.begin();
  while (current != end) {
    if (*current == '[') {
      ++current;
      if (*current == '%')
        addPathInArg(path, in, itInArg, PathArgument::kindIndex);
      else {
        ArrayIndex index = 0;
        for (; current != end && *current >= '0' && *current <= '9'; ++current)
          index = index * 10 + ArrayIndex(*current - '0');
        args_.push_back(index);
      }
      if (current == end || *current++ != ']')
        invalidPath(path, int(current - path.c_str()));
    } else if (*current == '%') {
      addPathInArg(path, in, itInArg, PathArgument::kindKey);
      ++current;
    } else if (*current == '.') {
      ++current;
    } else {
      const char* beginName = current;
      while (current != end && !strchr("[.", *current))
        ++current;
      args_.push_back(std::string(beginName, current));
    }
  }
}

void Path::addPathInArg(const std::string& /*path*/,
                        const InArgs& in,
                        InArgs::const_iterator& itInArg,
                        PathArgument::Kind kind) {
  if (itInArg == in.end()) {
    // Error: missing argument %d
  } else if ((*itInArg)->kind_ != kind) {
    // Error: bad argument type
  } else {
    args_.push_back(**itInArg);
  }
}

void Path::invalidPath(const std::string& /*path*/, int /*location*/) {
  // Error: invalid path.
}

const Value& Path::resolve(const Value& root) const {
  const Value* node = &root;
  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
    const PathArgument& arg = *it;
    if (arg.kind_ == PathArgument::kindIndex) {
      if (!node->isArray() || !node->isValidIndex(arg.index_)) {
        // Error: unable to resolve path (array value expected at position...
      }
      node = &((*node)[arg.index_]);
    } else if (arg.kind_ == PathArgument::kindKey) {
      if (!node->isObject()) {
        // Error: unable to resolve path (object value expected at position...)
      }
      node = &((*node)[arg.key_]);
      if (node == &Value::null) {
        // Error: unable to resolve path (object has no member named '' at
        // position...)
      }
    }
  }
  return *node;
}

Value Path::resolve(const Value& root, const Value& defaultValue) const {
  const Value* node = &root;
  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
    const PathArgument& arg = *it;
    if (arg.kind_ == PathArgument::kindIndex) {
      if (!node->isArray() || !node->isValidIndex(arg.index_))
        return defaultValue;
      node = &((*node)[arg.index_]);
    } else if (arg.kind_ == PathArgument::kindKey) {
      if (!node->isObject())
        return defaultValue;
      node = &((*node)[arg.key_]);
      if (node == &Value::null)
        return defaultValue;
    }
  }
  return *node;
}

Value& Path::make(Value& root) const {
  Value* node = &root;
  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
    const PathArgument& arg = *it;
    if (arg.kind_ == PathArgument::kindIndex) {
      if (!node->isArray()) {
        // Error: node is not an array at position ...
      }
      node = &((*node)[arg.index_]);
    } else if (arg.kind_ == PathArgument::kindKey) {
      if (!node->isObject()) {
        // Error: node is not an object at position...
      }
      node = &((*node)[arg.key_]);
    }
  }
  return *node;
}

} // namespace Json
