#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject){}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING UnicodeString){
    UNICODE_STRING path;
    UNICODE_STRING name;
    UNICODE_STRING data;
    OBJECT_ATTRIBUTES oa;
    HANDLE myhandle = NULL;

    RtlInitUnicodeString(&path, L"\\Registry\\Machine\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");

    RtlInitUnicodeString(&name, L"demon");
    RtlInitUnicodeString(&data, L"hello,demon");

    InitializeObjectAttributes(&oa, &path, OBJ_CASE_INSENSITIVE,NULL, NULL);
    ZwOpenKey(&myhandle, KEY_WRITE, &oa);
    ZwSetValueKey(myhandle, &name, 0, REG_SZ, data.Buffer, data.Length);

    ZwClose(myhandle);
    DriverObject->DriverUnload = Unload;
    return STATUS_SUCCESS;

}
