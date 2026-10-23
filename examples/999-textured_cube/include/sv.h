#ifndef CARR_SV_H_
#define CARR_SV_H_

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// The user can define this macro to include only the macro functions 
// with the 'carr_' prefix, as to avoid name collisions.
// If this macro is not defined, all the versions without prefix
// will be included by default.
#ifndef CARR_SV_FORCE_PREFIX

#define StringBuilder    CarrStringBuilder
#define sb_new           carr_sb_new
#define sb_with_cap      carr_sb_with_cap
#define sb_from_file     carr_sb_from_file
#define sb_realloc       carr_sb_realloc
#define sb_grow_cap      carr_sb_grow_cap
#define sb_grow          carr_sb_grow
#define sb_append        carr_sb_append
#define sb_nconcat       carr_sb_nconcat
#define sb_concat        carr_sb_concat
#define sb_concatf       carr_sb_concatf
#define sb_free          carr_sb_free


#define StringView       CarrStringView

#define sv_from_sb       carr_sv_from_sb
#define sv_from_cstr     carr_sv_from_cstr
#define sv_null          carr_sv_null
#define sv_first         carr_sv_first
#define sv_last          carr_sv_last
#define sv_chop_by_space carr_sv_chop_by_space
#define sv_chop_line     carr_sv_chop_line
#define sv_chop_by_delim carr_sv_chop_by_delim
#define sv_chop_left     carr_sv_chop_left
#define sv_chop_right    carr_sv_chop_right
#define sv_strip_space   carr_sv_strip_space
#define sv_trim_right    carr_sv_trim_right
#define sv_trim_left     carr_sv_trim_left
#define sv_is_equal      carr_sv_is_equal
#define sv_starts_with   carr_sv_starts_with
#define sv_printn        carr_sv_printn
#define sv_print         carr_sv_print
#define sv_to_cstr       carr_sv_to_cstr
#define sv_parse_int     carr_sv_parse_int

#endif  // CARR_SV_FORCE_PREFIX

#ifndef CARR_SV_TEMP_STR_SIZE
#define CARR_SV_TEMP_STR_SIZE 100
#endif  //CARR_SV_TEMP_STR_SIZE

#ifndef CARR_SB_INITIAL_CAP
#define CARR_SB_INITIAL_CAP   256
#endif  //CARR_SB_INITIAL_CAP

typedef struct {
    const char* data;
    size_t      len;
} CarrStringView;

typedef struct {
    char*   data;
    size_t  len;
    size_t  cap;
} CarrStringBuilder;

CarrStringBuilder carr_sb_new();
CarrStringBuilder carr_sb_with_cap(size_t cap);
CarrStringBuilder carr_sb_from_file(const char* file_path);
void              carr_sb_realloc(CarrStringBuilder* sb, size_t new_size);
size_t            carr_sb_grow_cap(CarrStringBuilder sb);
void              carr_sb_grow(CarrStringBuilder* sb);
void              carr_sb_free(CarrStringBuilder* sb);
void              carr_sb_append(CarrStringBuilder* sb, char ch);
void              carr_sb_nconcat(CarrStringBuilder* sb, const char* str, size_t n);
void              carr_sb_concat(CarrStringBuilder* sb, const char* cstr);
void              carr_sb_concatf(CarrStringBuilder* sb, const char* format, ...);

CarrStringView carr_sv_from_sb(CarrStringBuilder sb);
CarrStringView carr_sv_from_cstr(const char* in);
CarrStringView carr_sv_null();
char           carr_sv_first(CarrStringView* in);
char           carr_sv_last(CarrStringView* in);
CarrStringView carr_sv_chop_by_space(CarrStringView* in);
CarrStringView carr_sv_chop_line(CarrStringView* in);
CarrStringView carr_sv_chop_by_delim(CarrStringView* in, char delim);
char           carr_sv_chop_left(CarrStringView* in);
char           carr_sv_chop_right(CarrStringView* in);
void           carr_sv_strip_space(CarrStringView* in);
void           carr_sv_trim_right(CarrStringView* in, char sym);
void           carr_sv_trim_left(CarrStringView* in, char sym);
bool           carr_sv_is_equal(CarrStringView sv, CarrStringView other);
bool           carr_sv_starts_with(CarrStringView sv, const char* prefix);
void           carr_sv_printn(CarrStringView in);
void           carr_sv_print(CarrStringView in);
char*          carr_sv_to_cstr(CarrStringView in);
int            carr_sv_parse_int(CarrStringView* in);

// #define CARR_SV_IMPLEMENTATION
#ifdef CARR_SV_IMPLEMENTATION

CarrStringBuilder carr_sb_new()
{
    return (CarrStringBuilder) {
        .data = NULL,
        .len  = 0, 
        .cap  = 0,
    };
}

CarrStringBuilder carr_sb_with_cap(size_t cap) 
{
    CarrStringBuilder sb = carr_sb_new();
    carr_sb_realloc(&sb, cap);
    sb.cap = cap;
    return sb;
}

CarrStringBuilder carr_sb_from_file(const char* file_path)
{
    FILE* f = fopen(file_path, "r");
    if (f == NULL) {
        printf(
            "%s:%d:ERROR: sv_from_file: failed to open file '%s': %s\n",
            __FILE_NAME__, __LINE__, file_path, strerror(errno)
        );
        // if error => sb->data == NULL 
        return sb_new();
    }

    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    fseek(f, 0, SEEK_SET);

    CarrStringBuilder sb = carr_sb_with_cap(n);
    fread(sb.data, 1, n, f);
    fclose(f);
    sb.len = n;
    return sb;
}

void carr_sb_realloc(CarrStringBuilder* sb, size_t new_size)
{
    sb->data = (char*)realloc(sb->data, new_size);
}

size_t carr_sb_grow_cap(CarrStringBuilder sb)
{
    size_t new_cap;
    if (sb.cap == 0) {
        new_cap = CARR_SB_INITIAL_CAP;
    } else {
        new_cap = sb.cap * 2;
    }
    return new_cap;
}

void carr_sb_grow(CarrStringBuilder* sb)
{
    size_t new_cap = carr_sb_grow_cap(*sb);
    carr_sb_realloc(sb, new_cap * sizeof(char));
    sb->cap = new_cap;
}

void carr_sb_free(CarrStringBuilder* sb)
{
    free(sb->data);
    sb->len = 0;
    sb->cap = 0;
}

void carr_sb_append(CarrStringBuilder* sb, char ch)
{
    if (sb->len + 1 > sb->cap) {
        carr_sb_grow(sb);
    }
    sb->data[sb->len++] = ch;
}

void carr_sb_nconcat(CarrStringBuilder* sb, const char* str, size_t n)
{
    for (size_t idx = 0; idx < n; idx++) {
        char ch = str[idx];
        carr_sb_append(sb, ch);
    }
}

void carr_sb_concat(CarrStringBuilder* sb, const char* cstr)
{
    for (size_t idx = 0; cstr[idx] != '\0'; idx++) {
        char ch = cstr[idx];
        carr_sb_append(sb, ch);
    }
}

void carr_sb_concatf(CarrStringBuilder* sb, const char* format, ...)
{
    char temp[CARR_SV_TEMP_STR_SIZE];
    
    va_list args;
    va_start(args, format);
    vsnprintf(temp, CARR_SV_TEMP_STR_SIZE, format, args);
    va_end(args);

    sb_concat(sb, (const char*)temp);
}


CarrStringView carr_sv_from_sb(CarrStringBuilder sb)
{
    return (CarrStringView) {
        .data = sb.data,
        .len  = sb.len,
    };
}

CarrStringView carr_sv_from_cstr(const char* in)
{
    return (CarrStringView){
        .data = in,
        .len  = strlen(in),
    };
}

char *carr_sv_to_cstr(CarrStringView in)
{
    char *out = (char*)malloc((in.len + 1) * sizeof(char));
    sprintf(out, "%.*s", (int)in.len, in.data);
    return out;
}

CarrStringView carr_sv_null()
{
    return (CarrStringView){
        .data = NULL,
        .len  = 0,
    };
}

void carr_sv_print(CarrStringView in)
{
    printf("%.*s", (int)in.len, in.data);
}

void carr_sv_printn(CarrStringView in)
{
    printf("%.*s\n", (int)in.len, in.data);
}

char carr_sv_first(CarrStringView* in)
{
    assert(in->len > 0);
    return *in->data;
}

char carr_sv_last(CarrStringView* in)
{
    assert(in->len > 0);
    return in->data[in->len - 1];
}

void carr_sv_trim_left(CarrStringView* in, char sym)
{
    if (in->len == 0) {
        return;
    }

    while (*in->data == sym) {
        in->data++;
        in->len--;
    }
}

void carr_sv_trim_right(CarrStringView* in, char sym)
{
    if (in->len == 0) {
        return;
    }

    while (in->data[in->len - 1] == sym) {
        in->len--;
    }
}

void carr_sv_strip_space(CarrStringView* in)
{
    carr_sv_trim_left(in, ' ');
    carr_sv_trim_right(in, ' ');
}

bool carr_sv_is_equal(CarrStringView sv, CarrStringView other)
{
    if (sv.len != other.len) {
        return false;
    }
    for (size_t i = 0; i < sv.len; ++i) {
        if (sv.data[i] != other.data[i]) {
            return false;
        }
    }
    return true;
}

bool carr_sv_starts_with(CarrStringView sv, const char* prefix)
{
    size_t n = strlen(prefix);
    if (n > sv.len) {
        return false;
    }
    for (int i = 0; i < n; ++i) {
        if (sv.data[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}


CarrStringView carr_sv_chop_by_delim(CarrStringView* in, char delim)
{
    CarrStringView out = {0};
    for (size_t i = 0; i < in->len; ++i) {
        if (in->data[i] == delim) {
            out.data = in->data;
            out.len  = i;

            in->data += i + 1;
            in->len  -= i + 1;

            return out;
        };
    }
    out = *in;
    in->data = NULL;
    in->len  = 0;
    return out;
}

CarrStringView carr_sv_chop_line(CarrStringView* in)
{
    return carr_sv_chop_by_delim(in, '\n');
}

CarrStringView carr_sv_chop_by_space(CarrStringView* in)
{
    return carr_sv_chop_by_delim(in, ' ');
}

char carr_sv_chop_left(CarrStringView* in) 
{
    if (in->len == 0) return '\0';
    char ch = *in->data;
    in->data++;
    in->len--;
    return ch;
}

char carr_sv_chop_right(CarrStringView* in)
{
    if (in->len == 0) return '\0';
    char ch = in->data[in->len - 1];
    in->len--;
    return ch;
}

int carr_sv_parse_int(CarrStringView* in)
{
    assert(
        in->len > 0 && (
            isdigit(in->data[0]) ||
            in->data[0] == '-'
        )
    );
    bool is_neg = false;
    char ch = carr_sv_chop_left(in);
    if (ch == '-') {
        is_neg = true;
        ch = carr_sv_chop_left(in);
    }

    int sum = 0;
    while (in->len > 0) {
        if (ch < 48 || ch > 57) {
            break;
        }
        
        int n = ch - '0';
        sum *= 10;
        sum += n;
        ch = carr_sv_chop_left(in);
    }
    int n = ch - '0';
    sum *= 10;
    sum += n;
    in->data--;
    in->len++;
    if (is_neg) {
        sum = -sum;
    }
    return sum;
}

#endif // CARR_SV_IMPLEMENTATION
#endif // CARR_SV_H_
