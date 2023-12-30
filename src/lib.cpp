// This lib.cpp file is from the Professors solution of homework 7

#include <vector>
#include <stdio.h>
#include<stdlib.h>
#include "pack109.hpp"
#include <iostream>

using std::begin, std::end;
#define X8  256
#define X16 65536
#define MASK4 0x000000FF
#define MASK8 0x00000000000000FF

vec slice(vec& bytes, int vbegin, int vend) {
  auto start = bytes.begin() + vbegin;
  auto end = bytes.begin() + vend + 1;
  vec result(vend - vbegin + 1);
  copy(start, end, result.begin());
  return result;
}

// ----------------------------------------
// PACK109_TRUE and PACK109_FALSE
// ----------------------------------------

// We only need one serialize function for bool types, as we will test on the 
// value within the function, and push a tag depending on the value found.
vec pack109::serialize(bool item) {
  vec bytes;
  if (item == true) {
    bytes.push_back(PACK109_TRUE);
  } else {
    bytes.push_back(PACK109_FALSE);
  }
  return bytes;
}

bool pack109::deserialize_bool(vec bytes) {
  // If there are fewer than 1 bytes, we can't deserialize this vector. The 
  // bool message
  if (bytes.size() < 1) {
     throw;
  }

  // Here's why we make the previous check: because we are indexing into the
  // vector with a static value. If the vector doesn't contain at least
  // one element, the program could segfault.
  if (bytes[0] == PACK109_TRUE) {
    return true;
  } else if (bytes[0] == PACK109_FALSE) {
    return false;
  } else {
    // We throw if we are trying to deserialize a bool but the message is
    // not in fact a bool.
    throw;
  }

}

// ----------------------------------------
// PACK109_U8
// ----------------------------------------

// Serializing a u8 is simple: push the tag onto the vector, then the byte.
vec pack109::serialize(u8 item) {
  vec bytes;
  bytes.push_back(PACK109_U8);
  bytes.push_back(item);
  return bytes;
}

u8 pack109::deserialize_u8(vec bytes) {
  // To deserialize the u8, we need at least two bytes in the serialized
  // vector: one for the tag and one for the byte.
  if (bytes.size() < 2) {
    throw;
  }
  // Check for the correct tag
  if (bytes[0] == PACK109_U8) {
    // Directly return the byte. We can do this because the return type of the
    // function is the same as the contents of the vector: u8.
    return bytes[1];
  } else {
    // Throw if the tag is not a u8
    throw;
  }
}

// ----------------------------------------
// PACK109_U32
// ----------------------------------------

vec pack109::serialize(u32 item) {
  vec bytes;
  bytes.push_back(PACK109_U32);
  u32 mask = MASK4; // This mask will zero out the first 3 bytes
  // Look over the bytes of the integer. Integers have 4 bytes total.
  for(int i = 3; i >= 0; i--) {
    u32 to_shift = 8 * i; // Number of bytes to shift the item
    u32 shifted = item >> to_shift;
    // Mask the shifted bytes and cast as a char.
    // e.g. if the item is 0x12345678, and we shift 2 bytes:
    // shifted = 0x00001234
    // After the mask we have 0x00000034
    // After the cast we have 0x34. This value is pushed onto the vector.
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}

u32 pack109::deserialize_u32(vec bytes) {
  // The first byte is the tag, then we have 4 bytes for the payload, for a
  // total of 5 bytes
  if (bytes.size() < 5) {
    throw;
  }
  // To deserialize the u32, we need a container for it. 
  u32 deserialized_u32 = 0;
  if (bytes[0] == PACK109_U32) {
    int ix = 1;
    for(int i = 3; i >= 0; i--) {
      int to_shift = 8 * i;
      // We have to make sure to cast the byte to a u32 before shifting it.
      // If we don't, the bytes we are interested in will fall off:
      // e.g. if the byte is 0xAB and we want to left shift 2 bytes
      // bytes[ix] << to_shift = 0x00
      // (u32)bytes[ix] << to_shift = 0x00AB0000
      u32 shifted = (u32)bytes[ix] << to_shift;
      // Or is used here to put the shifted bytes into the container we
      // allocated outside of the for loop. e.g. if the byte vector is:
      // [AB,CD,EF,12]
      // Then deserialized_u32 will be:
      // 0xAB000000
      // 0xABCD0000
      // 0xABCDEF00
      // 0xABCDEF12
      // Over the 4 iterations of the loop
      deserialized_u32 = deserialized_u32 | shifted;
      ix++;
    }
    return deserialized_u32;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_U64
// ----------------------------------------

// Serializing a u64 works pretty much the same as serializing a u32. The big
// change is how many times we go through the for loop.
vec pack109::serialize(u64 item) {
  vec bytes;
  bytes.push_back(PACK109_U64);
  u64 mask = MASK8;
  for(int i = 7; i >= 0; i--) {
    int to_shift = 8 * i;
    u64 shifted = item >> to_shift;
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}

// Deserializing the u64 works the same as the u32. The big change here
// is the size of the container.
u64 pack109::deserialize_u64(vec bytes) {
  if (bytes.size() < 9) {
    throw;
  }
  u64 deserialized_u64 = 0; // This is a u64 now, so we can fit all the bytes
  if (bytes[0] == PACK109_U64) {
    int ix = 1;
    for(int i = 7; i >= 0; i--) {
      int to_shift = 8 * i;
      u64 shifted = (u64)bytes[ix] << to_shift;
      deserialized_u64 = deserialized_u64 | shifted;
      ix++;
    }
    return deserialized_u64;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_I8
// ----------------------------------------

vec pack109::serialize(i8 item) {
  vec bytes;
  bytes.push_back(PACK109_I8);
  // We can cast the i8 to a u8 and push because it's just a byte
  bytes.push_back((u8)item); 
  return bytes;
}

i8 pack109::deserialize_i8(vec bytes) {
  if (bytes.size() < 2) {
    throw;
  }
  if (bytes[0] == PACK109_I8) {
    // Get the byte and cast it to the correct return type
    return (i8)bytes[1];
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_I32
// ----------------------------------------

vec pack109::serialize(i32 item) {
  vec bytes;
  bytes.push_back(PACK109_I32);
  i32 mask = MASK4; 
  for(int i = 3; i >= 0; i--) {
    i32 to_shift = 8 * i;
    i32 shifted = item >> to_shift;
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}

i32 pack109::deserialize_i32(vec bytes) {
  if (bytes.size() < 5) {
    throw;
  }
  i32 deserialized_i32 = 0;
  if (bytes[0] == PACK109_I32) {
    int ix = 1;
    for(int i = 3; i >= 0; i--) {
      int to_shift = 8 * i;
      i32 shifted = (i32)bytes[ix] << to_shift;
      deserialized_i32 = deserialized_i32 | shifted;
      ix++;
    }
    return deserialized_i32;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_I64
// ----------------------------------------

vec pack109::serialize(i64 item) {
  vec bytes;
  bytes.push_back(PACK109_I64);
  i64 mask = MASK8;
  for(int i = 7; i >= 0; i--) {
    int to_shift = 8 * i;
    i64 shifted = item >> to_shift;
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}

i64 pack109::deserialize_i64(vec bytes) {
  if (bytes.size() < 9) {
    throw;
  }
  i64 deserialized_i64 = 0; 
  if (bytes[0] == PACK109_I64) {
    int ix = 1;
    for(int i = 7; i >= 0; i--) {
      int to_shift = 8 * i;
      i64 shifted = (i64)bytes[ix] << to_shift;
      deserialized_i64 = deserialized_i64 | shifted;
      ix++;
    }
    return deserialized_i64;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_F32
// ----------------------------------------

vec pack109::serialize(f32 item) {
  // The trick with the f32 is to get a reference to the underlying bytes,
  // and then cast it to an unsigned int pointer. Then everything else
  // works the same as serialize i32.
  unsigned int* f32_pointer = (unsigned int*) (&item);
  vec bytes;
  bytes.push_back(PACK109_F32);
  u32 mask = MASK4;
  for(int i = 3; i >= 0; i--) {
    int to_shift = 8 * i;
    u32 shifted = *f32_pointer >> to_shift;
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}


f32 pack109::deserialize_f32(vec bytes) {
  if (bytes.size() < 5) {
    throw;
  }
  // We deserialize the f32 the same way as an u32, and then cast it to
  // an f32 on return.
  u32 deserialized_u32 = 0;
  if (bytes[0] == PACK109_F32) {
    int ix = 1;
    for(int i = 3 ; i >= 0; i--) {
      int to_shift = 8 * i;
      u32 shifted = (u32)bytes[ix] << to_shift;
      deserialized_u32 = deserialized_u32 | shifted;
      ix++;
    }
    f32* deserialized_f32 = (f32*) (&deserialized_u32);
    return *deserialized_f32;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_F64
// ----------------------------------------


vec pack109::serialize(f64 item) {
  // We use the same trick here as in f32, but instead we cast it to an u64*
  u64* f64_pointer = (u64*)(&item);
  vec bytes;
  bytes.push_back(PACK109_F64);
  u64 mask = MASK8;
  for(int i =7; i >= 0; i--) {
    int to_shift = 8 * i;
    u64 shifted = *f64_pointer >> to_shift;
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}

f64 pack109::deserialize_f64(vec bytes) {
  if (bytes.size() < 9) {
    throw;
  }
  u64 deserialized_u64 = 0;
  if (bytes[0] == PACK109_F64) {
    int ix = 1;
    for(int i = 7; i >= 0; i--) {
      int to_shift = 8 * i;
      u64 shifted = (u64)bytes[ix] << to_shift;
      deserialized_u64 = deserialized_u64 | shifted;
      ix++;
    }
    f64* deserialized_f64 = (f64*) (&deserialized_u64);
    return *deserialized_f64;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_S8 and PACK109_S16
// ----------------------------------------

// We can handle S8 and S16 in a single function by checking the length of the
// input string. If it's fewer than 256 characters, we can output a serialized
// S8. If it's up to 2^16, we can output a serialized S16.

vec pack109::serialize(string item) {
  vec bytes;
  if (item.size() < 256) {
    bytes.push_back(PACK109_S8);
    bytes.push_back((u8)item.size());
    // Push each byte of the string onto the vector
    for (int i = 0; i < item.size(); i++) {
      bytes.push_back(item[i]);
    }
  } else if (item.size() < 65536) {
    bytes.push_back(PACK109_S16);
    u32 string_length = (u32)item.size();
    // Push the first byte of the length onto the vector
    bytes.push_back((u8)(string_length >> 8));
    // Push the second byte of the length onto the vector 
    bytes.push_back((u8)(string_length));
    // Push each byte of the string onto the vector
    for (int i = 0; i < item.size(); i++) {
      bytes.push_back((u8)item[i]);
    }
  } else {
    throw;
  }
  return bytes;
}

string pack109::deserialize_string(vec bytes) {
  if(bytes.size() < 3) {
    throw;
  }
  string deserialized_string = "";
  if(bytes[0] == PACK109_S8) {
    // The string length is byte 1
    int string_length = bytes[1];
    // The string starts at byte 2
    for(int i = 2; i < (string_length + 2); i++) {
      deserialized_string += bytes[i];
    }
  }
  else if(bytes[0]==PACK109_S16) {
    // Reconstruct the string length from bytes 1 and 2
    int string_length = (bytes[1]<<8) | bytes[2];
    // The string starts at byte 3
    for(int i = 3; i < (string_length + 3); i++) {
      deserialized_string += bytes[i];
    }
  }
  return deserialized_string;
}
  
// ----------------------------------------
// PACK109_A8 and PACK109_A16
// ----------------------------------------

// Each of the following functions will be very similar. There are two cases
// for the serialize function: one for the x8 and one for the x16 objects.
// In each function, we can leverage the serde functions implemented above
// to convert items and serialized objects from one form to another.

vec pack109::serialize(std::vector<u8> item) {
  vec bytes;
  if (item.size() < X8) {
    bytes.push_back(PACK109_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++) {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++) {
        bytes.push_back(temp[j]);
      }
    }
  } else if (item.size() < X16) {
    bytes.push_back(PACK109_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++) {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  } else {
    throw;
  }
  return bytes;
}

vec pack109::serialize(std::vector<u64> item) {
  vec bytes;
  if (item.size() < X8) {
    bytes.push_back(PACK109_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++) {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++) {
        bytes.push_back(temp[j]);
      }
    }
  } else if (item.size() < X16) {
    bytes.push_back(PACK109_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++) {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  } else {
    throw;
  }
  return bytes;
}

vec pack109::serialize(std::vector<f64> item) {
  vec bytes;
  if (item.size() < X8) {
    bytes.push_back(PACK109_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++) {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++) {
        bytes.push_back(temp[j]);
      }
    }
  } else if (item.size() < X16) {
    bytes.push_back(PACK109_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++) {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  } else {
    throw;
  }
  return bytes;
}

vec pack109::serialize(std::vector<string> item) {
  vec bytes;
  if (item.size() < X8) {
    bytes.push_back(PACK109_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++) {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++) {
        bytes.push_back(temp[j]);
      }
    }
  } else if (item.size() < X16) {
    bytes.push_back(PACK109_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++) {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  } else {
    throw;
  }
  return bytes;
}

std::vector<u8> pack109::deserialize_vec_u8(vec bytes) {
  if(bytes.size() < 3) {
    throw;
  }
  int el_size = 2;
  std::vector<u8> result;
  if(bytes[0] == PACK109_A8) {
    int size = el_size * bytes[1];
    for (int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  } else if(bytes[0] == PACK109_A16) {
    int size = el_size * (((int)bytes[1])<<8 | (int)bytes[2]);
    for(int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i + 1, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  }
  return result;
}

std::vector<u64> pack109::deserialize_vec_u64(vec bytes) {
  if(bytes.size() < 3) {
    throw;
  }
  int el_size = 9;
  std::vector<u64> result;
  if(bytes[0] == PACK109_A8) {
    // Each u64 element is 9 bytes long (8 bytes payload + 1 tag byte), so
    // the total size of the payload is size byte (byte 1) multiplied by 9
    int size = el_size * bytes[1];
    // The payload starts at byte 2. We increment by 9 bytes every
    // iteration
    for (int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i, i + el_size);
      u64 element = deserialize_u64(sub_vec);
      result.push_back(element);
    }
  } else if(bytes[0] == PACK109_A16) {
    int size = el_size * (((int)bytes[1])<<8 | (int)bytes[2]);
    for(int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i + 1, i + el_size);
      u64 element = deserialize_u64(sub_vec);
      result.push_back(element);
    }
  }
  return result;
}

std::vector<f64> pack109::deserialize_vec_f64(vec bytes) {
  if(bytes.size() < 3) {
    throw;
  }
  int el_size = 9;
  std::vector<f64> result;
  if(bytes[0] == PACK109_A8) {
    int size = el_size * bytes[1];
    for (int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i, i + el_size - 1);
      f64 element = deserialize_f64(sub_vec);
      result.push_back(element);
    }
  } else if(bytes[0] == PACK109_A16) {
    int size = el_size * (((int)bytes[1])<<8 | (int)bytes[2]);
    for(int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i + 1, i + el_size);
      f64 element = deserialize_f64(sub_vec);
      result.push_back(element);
    }
  }
  return result;
}

vec pack109::serialize(struct Person item) {
  vec bytes;
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x1); // 1 k/v pair
  // The key is "Person"
  vec person = serialize(string("Person"));
  bytes.insert(end(bytes), begin(person), end(person));
  // The value is an m8
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x3); // 3 k/v pairs
  // k/v 1 is "age"
  vec agek = serialize(string("age"));
  bytes.insert(end(bytes), begin(agek), end(agek));
  vec agev = serialize(item.age);
  bytes.insert(end(bytes), begin(agev), end(agev));
  // k/v 2 is "height"
  vec heightk = serialize(string("height"));
  bytes.insert(end(bytes), begin(heightk), end(heightk));
  vec heightv = serialize(item.height);
  bytes.insert(end(bytes), begin(heightv), end(heightv));
  // k/v 2 is "name"
  vec namek = serialize(string("name"));
  bytes.insert(end(bytes), begin(namek), end(namek));
  vec namev = serialize(item.name);
  bytes.insert(end(bytes), begin(namev), end(namev));
  return bytes;
}

struct Person pack109::deserialize_person(vec bytes) {
  if (bytes.size() < 10) {
    throw;
  }
  vec person_slice = slice(bytes, 2, 9);
  string person_string = deserialize_string(person_slice);
  if (person_string != "Person") {
    throw;
  }
  vec agev = slice(bytes, 17, 18);
  u8 age = deserialize_u8(agev);
  vec heightv = slice(bytes, 27, 32);
  f32 height=deserialize_f32(heightv);
  u8 name_len = bytes[39]; // Assumes name fewer than 256 chars. Safe assumption I think
  vec namev = slice(bytes, 38, 38 + name_len + 1);
  string name = deserialize_string(namev);
  struct Person deserialized_person = {age, height, name};
  return deserialized_person;
}


//This is the code that I got from slack that was released by the professor
vec pack109::serialize(struct File item) {
    vec result;
    result.push_back(PACK109_M8);
    result.push_back(0x01);
    auto key1 = pack109::serialize(string{"File"});
    result.insert(result.end(), key1.begin(), key1.end());
    result.push_back(PACK109_M8);
    result.push_back(0x02);
    auto key2_1 = pack109::serialize(string{"name"});
    result.insert(result.end(), key2_1.begin(), key2_1.end());
    auto value2_1 = pack109::serialize(item.name);
    result.insert(result.end(), value2_1.begin(), value2_1.end());
    auto key2_2 = pack109::serialize(string{"bytes"});
    result.insert(result.end(), key2_2.begin(), key2_2.end());
    auto value2_2 = pack109::serialize(item.bytes);
    result.insert(result.end(), value2_2.begin(), value2_2.end());
    return result;
}

vec pack109::serialize(struct Request item) {
    vec result;
    result.push_back(PACK109_M8);
    result.push_back(0x01);
    auto key1 = pack109::serialize(string{"Request"});
    result.insert(result.end(), key1.begin(), key1.end());
    result.push_back(PACK109_M8);
    result.push_back(0x01);
    auto key2_1 = pack109::serialize(string{"name"});
    result.insert(result.end(), key2_1.begin(), key2_1.end());
    auto value2_1 = pack109::serialize(item.name);
    result.insert(result.end(), value2_1.begin(), value2_1.end());
    return result;
}

struct File pack109::deserialize_file(vec bytes) {
    struct File result;
    unsigned int i = 2;
    string pair_1_key_1 = pack109::deserialize_string(vec(bytes.begin() + i, bytes.end()));
    i += (2 + pair_1_key_1.size() + 2);
    string pair_2_key_1 = pack109::deserialize_string(vec(bytes.begin() + i, bytes.end()));
    i += (pair_2_key_1.size() + 2);
    string filename = pack109::deserialize_string(vec(bytes.begin() + i, bytes.end()));
    result.name = filename;
    i += (filename.size() + 2 + 7);
    vec binary = deserialize_vec_u8(vec(bytes.begin() + i, bytes.end()));
    result.bytes = binary;
    return result;
}

struct Request pack109::deserialize_request(vec bytes) {
    struct Request result;
    unsigned int i = 2;
    string pair_1_key_1 = pack109::deserialize_string(vec(bytes.begin() + i, bytes.end()));
    i += (2 + pair_1_key_1.size() + 2);
    string pair_2_key_1 = pack109::deserialize_string(vec(bytes.begin() + i, bytes.end()));
    i += (pair_2_key_1.size() + 2);
    string filename = pack109::deserialize_string(vec(bytes.begin() + i, bytes.end()));
    result.name = filename;
    return result;
}

void pack109::printVec(vec &bytes) {
  printf("[ ");
  for (int i = 0; i < bytes.size(); i++) {
    printf("%x ", bytes[i]);
  }
  printf("]\n");
}