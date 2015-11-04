//	scoped_process_information.h
//
//

#ifndef __BASE_WIN_SCOPED_PROCESS_INFORMATION_H__
#define __BASE_WIN_SCOPED_PROCESS_INFORMATION_H__

#include <windows.h>

#include "scoped_handle.h"

namespace base {
namespace win {

	// Manages the closing of process and thread handles from PROCESS_INFORMATION
	// structures. Allows clients to take ownership of either handle independently.
	class ScopedProcessInformation {
	public:
		// Helper object to contain the effect of Receive() to the funtion that needs
		// a pointer.
		class Receiver {
		public:
			explicit Receiver(ScopedProcessInformation* owner)
				: info_(),
				owner_(owner) {}
			~Receiver() { owner_->Set(info_); }

			operator PROCESS_INFORMATION*() { return &info_; }

		private:
			PROCESS_INFORMATION info_;
			ScopedProcessInformation* owner_;
		};

		ScopedProcessInformation();
		~ScopedProcessInformation();

		// Returns an object that may be passed to API calls such as CreateProcess.
		// DCHECKs that the object is not currently holding any handles.
		// HANDLEs stored in the returned PROCESS_INFORMATION will be owned by this
		// instance.
		// The intended use case is something like this:
		//   if (::CreateProcess(..., startup_info, scoped_proces_info.Receive()))
		Receiver Receive();

		// Returns true iff this instance is holding a thread and/or process handle.
		bool IsValid() const;

		// Closes the held thread and process handles, if any.
		void Close();

		// Populates this instance with the provided |process_info|.
		void Set(const PROCESS_INFORMATION& process_info);

		// Populates this instance with duplicate handles and the thread/process IDs
		// from |other|. Returns false in case of failure, in which case this instance
		// will be completely unpopulated.
		bool DuplicateFrom(const ScopedProcessInformation& other);

		// Transfers ownership of the held PROCESS_INFORMATION, if any, away from this
		// instance.
		PROCESS_INFORMATION Take();

		// Transfers ownership of the held process handle, if any, away from this
		// instance. Note that the related process_id will also be cleared.
		HANDLE TakeProcessHandle();

		// Transfers ownership of the held thread handle, if any, away from this
		// instance. Note that the related thread_id will also be cleared.
		HANDLE TakeThreadHandle();

		// Returns the held process handle, if any, while retaining ownership.
		HANDLE process_handle() const {
			return process_handle_.Get();
		}

		// Returns the held thread handle, if any, while retaining ownership.
		HANDLE thread_handle() const {
			return thread_handle_.Get();
		}

		// Returns the held process id, if any.
		DWORD process_id() const {
			return process_id_;
		}

		// Returns the held thread id, if any.
		DWORD thread_id() const {
			return thread_id_;
		}

	private:
		ScopedHandle process_handle_;
		ScopedHandle thread_handle_;
		DWORD process_id_;
		DWORD thread_id_;

		DISALLOW_COPY_AND_ASSIGN(ScopedProcessInformation);
	};

} // namespace base::win.
} // namespace base.

#endif // __BASE_WIN_SCOPED_PROCESS_INFORMATION_H__

