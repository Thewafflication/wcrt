# REQ-0100: Narrow-time enumeration with wide file sizes

io.h shall declare pointer-sized wcrt_findfirst32i64(const char *, struct
wcrt_finddata32i64 *) and int wcrt_findnext32i64(__wcrt_intptr_t, struct
wcrt_finddata32i64 *), default-cdecl. The pack-8 record has unsigned attributes
at 0, signed long times at 4/8/12, signed wcrt_ms_int64 size at 16, char name[260]
at 24, and total size 288; preserve caller packing. Accept file sizes through
INT64_MAX, rejecting greater native unsigned sizes with ERANGE without
truncation. Publish _fsize_t as unsigned long and use it in finddata32.

WCRT_VC6_FIND32 additionally selects x86 _finddatai64_t/_findfirsti64/_findnexti64
source spellings to the explicit interfaces. Default aliases remain absent;
existing generic binary ABI is not replaced. Both record forms share the
existing wcrt_findclose handle ownership and all REQ-0096 matching/time/error
rules. A handle acquired with either first operation can use either next
operation; conversion failure advances it but does not close it.

Correct REQ-0096 next behavior: every failed next operation preserves the
complete previous caller record, including exhaustion/invalid handles and
conversion overflow. Both next forms convert into temporary records and commit
only on success. First failures still zero supplied output and release newly
acquired native resources as specified by REQ-0096.
First calls shall read a valid specification before clearing output, allowing
the pattern to reside in the caller's result name array without being erased.

## Verification and impact

TC-0100 covers layouts/types, source aliases, mixed record searches, native
equivalence and deterministic wide-size boundaries/errors/guards/ownership.
Update TC-0096 and supplementary Microsoft checks for failed-next retention.
Rebuild packages and extend inventories, manifests, quality/traceability and
import gates. Existing first/close behavior and generic/default time ABI remain
unchanged; failed-next output retention is the explicit behavior correction.
Work record covers rationale/estimates/review. VC6/reference remains Unknown.
ADR-0010 records the durable versioned-record/source-selection decision.
