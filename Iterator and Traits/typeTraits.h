#pragma once

namespace STL {

	struct __true_type { };
	struct __false_type { };

	template <class T>
	struct __type_traits {
		using has_trivial_default_constructor = __false_type;
		using has_trivial_copy_constructor = __false_type;
		using has_trivial_assignment_constructor = __false_type;
		using has_trivial_destructor_constructor = __false_type;
		using is_POD_type = __false_type;
	};

	template <>
	struct __type_traits<signed char> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<short> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<unsigned short> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<int> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<unsigned int> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<long> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<unsigned long> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<float> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<double> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<long double> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<char> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<char> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};

	template <>
	struct __type_traits<char> {
		using has_trivial_default_constructor = __true_type;
		using has_trivial_copy_constructor = __true_type;
		using has_trivial_assignment_constructor = __true_type;
		using has_trivial_destructor_constructor = __true_type;
		using is_POD_type = __true_type;
	};
}
