// Saving and loading (kern/docs/app.md 5.): named slots of named values of the language, kept by the host - the
// page's localStorage in the browser, files natively. The Engine's save_* / load_* (kern/app/app.h) use it; a test
// gives the engine a host of its own (Engine::set_storage).
#pragma once

#include <string>
#include <string_view>

namespace kern {

/// Where slots are kept: the text of a slot by its name.
class StorageHost {
 public:
  virtual ~StorageHost() = default;
  /// The slot's text. False with `why` when the host cannot read; true with `found` false when there is none.
  virtual bool read(std::string_view slot, std::string& text, bool& found, std::string& why) = 0;
  /// Writes the slot's text (replacing it). False with `why` (full, not allowed, no such folder ...).
  virtual bool write(std::string_view slot, std::string_view text, std::string& why) = 0;
  /// Removes the slot (none: true).
  virtual bool remove(std::string_view slot, std::string& why) = 0;
};

/// The platform's host: in WebAssembly the page's (kern_env storage_*: localStorage), natively files
/// `<dir>/<slot>.kern-save` (the folder is made when the first slot is written).
StorageHost& platform_storage(std::string dir);

/// A slot's and a value's name: 1..64 of A-Z a-z 0-9 _ - .
bool storage_name_ok(std::string_view name);

}  // namespace kern
