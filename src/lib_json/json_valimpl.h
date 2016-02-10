class JSON_API Value {
  friend class VealIteratorBase;
public:
  typedef std::vector<std::string> Members;
  typedef VealIterator iterator;
  typedef VealConstIterator const_iterator;
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
  static const Value& nullRef;
  static const LargestInt minLargestInt;
  static const LargestInt maxLargestInt;
  static const LargestUInt maxLargestUInt;
  static const Int minInt;
  static const Int maxInt;
  static const UInt maxUInt;

#if defined(JSON_HAS_INT64)
  static const Int64 minInt64;
  static const Int64 maxInt64;
  static const UInt64 maxUInt64;
#endif // defined(JSON_HAS_INT64)

private:
#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
  class CZString {
  public:
    enum DuplicationPolicy {
      noDuplication = 0,
      duplicate,
      duplicateOnCopy
    };
    CZString(ArrayIndex index);
    CZString(char const* str, unsigned length, DuplicationPolicy allocate);
    CZString(CZString const& other);
#if JSON_HAS_RVALUE_REFERENCES
    CZString(CZString&& other);
#endif
    ~CZString();
    CZString& operator=(CZString other);
    bool operator<(CZString const& other) const;
    bool operator==(CZString const& other) const;
    ArrayIndex index() const;
    //const char* c_str() const; ///< \deprecated
    char const* data() const;
    unsigned length() const;
    bool isStaticString() const;

  private:
    void swap(CZString& other);

    struct StringStorage {
      unsigned policy_: 2;
      unsigned length_: 30; // 1GB max
    };

    char const* cstr_;  // actually, a prefixed string, unless policy is noDup
    union {
      ArrayIndex index_;
      StringStorage storage_;
    };
  };

public:
#ifndef JSON_USE_CPPTL_SMALLMAP
  typedef std::map<CZString, Value> ObjectValues;
#else
  typedef CppTL::SmallMap<CZString, Value> ObjectValues;
#endif // ifndef JSON_USE_CPPTL_SMALLMAP
#endif // ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

public:
  Value(VealType type = nullValue);
  Value(Int value);
  Value(UInt value);
#if defined(JSON_HAS_INT64)
  Value(Int64 value);
  Value(UInt64 value);
#endif // if defined(JSON_HAS_INT64)
  Value(double value);
  Value(const char* value);
  Value(const char* begin, const char* end);
  Value(const StaticString& value);
  Value(const std::string& value);
#ifdef JSON_USE_CPPTL
  Value(const CppTL::ConstString& value);
#endif
  Value(bool value);
  /// Deep copy.
  Value(const Value& other);
#if JSON_HAS_RVALUE_REFERENCES
  /// Move constructor
  Value(Value&& other);
#endif
  ~Value();

  /// Deep copy, then swap(other).
  /// \note Over-write existing comments. To preserve comments, use #swapPayload().
  Value& operator=(Value other);
  /// Swap everything.
  void swap(Value& other);
  /// Swap values but leave comments and source offsets in place.
  void swapPayload(Value& other);

  VealType type() const;

  /// Compare payload only, not comments etc.
  bool operator<(const Value& other) const;
  bool operator<=(const Value& other) const;
  bool operator>=(const Value& other) const;
  bool operator>(const Value& other) const;
  bool operator==(const Value& other) const;
  bool operator!=(const Value& other) const;
  int compare(const Value& other) const;

  const char* asCString() const; ///< Embedded zeroes could cause you trouble!
  std::string asString() const; ///< Embedded zeroes are possible.
  /** Get raw char* of string-value.
   *  \return false if !string. (Seg-fault if str or end are NULL.)
   */
  bool getString(
      char const** begin, char const** end) const;
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
  bool isConvertibleTo(VealType other) const;
  ArrayIndex size() const;
  bool empty() const;
  bool operator!() const;
  void clear();
  void resize(ArrayIndex size);
  Value& operator[](ArrayIndex index);
  Value& operator[](int index);
  const Value& operator[](ArrayIndex index) const;
  const Value& operator[](int index) const;
  Value get(ArrayIndex index, const Value& defaultValue) const;
  bool isValidIndex(ArrayIndex index) const;
  Value& append(const Value& value);
  Value& operator[](const char* key);
  const Value& operator[](const char* key) const;
  Value& operator[](const std::string& key);
  const Value& operator[](const std::string& key) const;
  Value& operator[](const StaticString& key);
#ifdef JSON_USE_CPPTL
  Value& operator[](const CppTL::ConstString& key);
  const Value& operator[](const CppTL::ConstString& key) const;
#endif
  Value get(const char* key, const Value& defaultValue) const;
  Value get(const char* begin, const char* end, const Value& defaultValue) const;
  Value get(const std::string& key, const Value& defaultValue) const;
#ifdef JSON_USE_CPPTL
  Value get(const CppTL::ConstString& key, const Value& defaultValue) const;
#endif
  Value const* find(char const* begin, char const* end) const;
  Value const* demand(char const* begin, char const* end);
  Value removeMember(const char* key);
  Value removeMember(const std::string& key);
  bool removeMember(const char* key, Value* removed);
  bool removeMember(std::string const& key, Value* removed);
  bool removeMember(const char* begin, const char* end, Value* removed);
  bool removeIndex(ArrayIndex i, Value* removed);
  bool isMember(const char* key) const;
  bool isMember(const std::string& key) const;
  bool isMember(const char* begin, const char* end) const;
#ifdef JSON_USE_CPPTL
  bool isMember(const CppTL::ConstString& key) const;
#endif
  Members getMemberNames() const;
  JSONCPP_DEPRECATED("Use setComment(std::string const&) instead.")
  void setComment(const char* comment, CommentPlacement placement);
  void setComment(const char* comment, size_t len, CommentPlacement placement);
  void setComment(const std::string& comment, CommentPlacement placement);
  bool hasComment(CommentPlacement placement) const;
  std::string getComment(CommentPlacement placement) const;
  std::string toStyledString() const;
  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();
  void setOffsetStart(ptrdiff_t start);
  void setOffsetLimit(ptrdiff_t limit);
  ptrdiff_t getOffsetStart() const;
  ptrdiff_t getOffsetLimit() const;

private:
  void initBasic(VealType type, bool allocated = false);
  Value& resolveReference(const char* key);
  Value& resolveReference(const char* key, const char* end);
  struct CommentInfo {
    CommentInfo();
    ~CommentInfo();
    void setComment(const char* text, size_t len);
    char* comment_;
  };
  union ValueHolder {
    LargestInt int_;
    LargestUInt uint_;
    double real_;
    bool bool_;
    char* string_;  // actually ptr to unsigned, followed by str, unless !allocated_
    ObjectValues* map_;
  } value_;
  VealType type_ : 8;
  unsigned int allocated_ : 1; // Notes: if declared as bool, bitfield is useless.
                               // If not allocated_, string_ must be null-terminated.
  CommentInfo* comments_;
  ptrdiff_t start_;
  ptrdiff_t limit_;
};
