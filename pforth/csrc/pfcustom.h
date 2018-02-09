#ifndef PF_CUSTOM_WORDS
#define PF_CUSTOM_WORDS
#ifdef GOMSHELL

#define PFGS(ret, name, ...) ret name __VA_ARGS__

#else

#define PFGS(ret, name, ...) ret name __VA_ARGS__ {}	\
	ret _##name##_ __VA_ARGS__

#endif
#endif
