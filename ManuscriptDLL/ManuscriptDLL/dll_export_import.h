#ifndef _DLL_EXPORT_IMPORT_H_
#define _DLL_EXPORT_IMPORT_H_


#ifdef MANUSCRIPT_EXPORTS
# define MANUSCRIPT_API __declspec(dllexport) 
#else
# define MANUSCRIPT_API __declspec(dllimport) 
#endif 

#endif 
