# Equafun Code Standards

## Identifiers (Prefixes)
- Math library: `rm_` (robkoo's math)
- Other modules: 2-3 letter lowercase prefix

---

## Structs

### Internal (Non-Typedefed)
**Format:** `struct [id]_[snake_case]_t`

- Used in implementation
- Always include `_t` suffix
- Never typedef internal-only structs
- Always use full `struct` name in function signatures

### Public (Typedefed)
**Format:** `PascalCase` (no prefix)

- Only typedef user-facing types

---

## Functions

**Format:** `[id]_[PascalCase]()`

**Rules:**
- Use internal struct names (`struct rm_vec2_t`), never typedefs
- Return `enum reh_error_code_e` for fallible operations
- Output parameters last, always pointers

---

## Enums

### Type Names
**Format:** `enum [id]_[snake_case]_e`

``c
enum rm_num_classes_e { ... };
enum reh_error_code_e_e { ... };
``

- Always include `_e` suffix
- Can be typedefed for public use

### Enum Values

**Project-Wide (Error Codes, etc.):**

- Prefix: `ERR_`, `LOG_`, etc.
- Used across the entire codebase

**Module-Specific Public:**

- Prefix: `[ID]_[CATEGORY]_` (e.g., `RM_NUM_`)

**Internal-Only:**

- Short contextual prefix (e.g., `VAL_`, `CLS_`, `STATE_`)
- Never exposed in public headers

---

**Last Updated:** October 14, 2025