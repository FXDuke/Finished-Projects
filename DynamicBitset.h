#include <iostream>
#include <sstream>
const uint64_t mINT64_MAX = 0x7FFFFFFFFFFFFFFF;
const uint64_t i64 = 64;
const uint32_t mINT32_MAX = 0x7FFFFFFF;
const uint32_t i32 = 32;
namespace DynamicBitset
{
    class b64 {

    private:

        uint64_t Size;
        uint64_t* Storage = new uint64_t[0];

    public:

        // @Overload

        bool operator==(DynamicBitset::b64& Target)
        {
            if (Target.getSize() != Size)
                return false;
            for (int Index = 0; Index < Size; Index++)
            {
                if (Target.getValue(Index) != Storage[Index])
                    return false;
            }
            return true;
        }

        bool operator!=(DynamicBitset::b64& Target)
        {
            if (Target.getSize() != Size)
                return true;
            for (int Index = 0; Index < Size; Index++)
            {
                if (Target.getValue(Index) != Storage[Index])
                    return true;
            }
            return false;
        }

        DynamicBitset::b64 operator|(DynamicBitset::b64& Target)
        {
            uint64_t TSize = Target.getSize();
            DynamicBitset::b64 Result(TSize * i64);
            for (uint64_t Index = 0; Index < TSize; Index++)
                Result.Storage[Index] = Storage[Index] | Target.getValue(Index);
            return Result;
        }

        DynamicBitset::b64 operator^(DynamicBitset::b64& Target)
        {
            uint64_t TSize = Target.getSize();
            DynamicBitset::b64 Result(TSize * i64);
            for (uint64_t Index = 0; Index < TSize; Index++)
                Result.Storage[Index] = Storage[Index] ^ Target.getValue(Index);
            return Result;
        }

        DynamicBitset::b64 operator&(DynamicBitset::b64& Target)
        {
            uint64_t TSize = Target.getSize();
            DynamicBitset::b64 Result(TSize * i64);
            for (uint64_t Index = 0; Index < TSize; Index++)
                Result.Storage[Index] = Storage[Index] & Target.getValue(Index);
            return Result;
        }

        DynamicBitset::b64 operator~()
        {
            DynamicBitset::b64 Result(Size * i64);
            for (uint64_t Index = 0; Index < Size; Index++)
                Result.Storage[Index] = mINT64_MAX ^ Storage[Index];
            return Result;
        }

        DynamicBitset::b64 operator<<(int Amount)
        {

            if (Amount < 0)
                return operator>>(-Amount);

            DynamicBitset::b64 Result(Size * i64);

            int ShiftAmount = Amount & 63;
            int Dif = 64 - ShiftAmount;
            Amount >>= 6;

            for (int Index = Size - 1; Index > -1; Index--)
                Result.Storage[Index] = (Index - Amount > -1) ? Storage[Index - Amount] : 0;

            for (int Index = Size - 1; Index > -1; Index--)
                Result.Storage[Index] = (Result.Storage[Index] << ShiftAmount) | ((Index - 1 > -1) ? Result.Storage[Index - 1] >> Dif : 0);

            return Result;
        }

        DynamicBitset::b64 operator>>(int Amount)
        {

            if (Amount < 0)
                return operator<<(-Amount);

            DynamicBitset::b64 Result(Size * i64);

            int ShiftAmount = Amount & 63;
            int Dif = 64 - ShiftAmount;
            Amount >>= 6;

            for (int Index = 0; Index < Size; Index++)
                Result.Storage[Index] = (Index + Amount < Size) ? Storage[Index + Amount] : 0;

            for (int Index = 0; Index < Size; Index++)
                Result.Storage[Index] = (Result.Storage[Index] >> ShiftAmount) | ((Index + 1 < Size) ? Result.Storage[Index + 1] << Dif : 0);

            return Result;
        }

        void operator<<=(int Amount)
        {

            if (Amount < 0)
                return operator>>=(-Amount);

            int ShiftAmount = Amount & 63;
            int Dif = 64 - ShiftAmount;
            Amount >>= 6;

            for (int Index = Size - 1; Index > -1; Index--)
                Storage[Index] = (Index - Amount > -1) ? Storage[Index - Amount] : 0;

            for (int Index = Size - 1; Index > -1; Index--)
                Storage[Index] = (Storage[Index] << ShiftAmount) | ((Index != 0) ? Storage[Index - 1] >> Dif : 0);

        }

        void operator>>=(int Amount)
        {

            if (Amount < 0)
                return operator<<=(-Amount);

            int ShiftAmount = Amount & 63;
            int Dif = 64 - ShiftAmount;
            Amount >>= 6;

            for (int Index = 0; Index < Size; Index++)
                Storage[Index] = (Index + Amount < Size) ? Storage[Index + Amount] : 0;

            for (int Index = 0; Index < Size; Index++)
                Storage[Index] = (Storage[Index] >> ShiftAmount) | ((Index + 1 < Size) ? Storage[Index + 1] << Dif : 0);
        }

        void operator|=(DynamicBitset::b64& Target)
        {
            uint64_t TSize = Target.getSize();
            for (uint64_t Index = 0; Index < TSize; Index++)
                Storage[Index] = Storage[Index] | Target.getValue(Index);
        }

        void operator^=(DynamicBitset::b64& Target)
        {
            uint64_t TSize = Target.getSize();
            for (uint64_t Index = 0; Index < TSize; Index++)
                Storage[Index] = Storage[Index] ^ Target.getValue(Index);
        }

        void operator&=(DynamicBitset::b64& Target)
        {
            uint64_t TSize = Target.getSize();
            for (uint64_t Index = 0; Index < TSize; Index++)
                Storage[Index] = Storage[Index] & Target.getValue(Index);
        }

        // @Read / Write
        void flip(uint64_t Position)
        {
            Storage[Position >> 6] ^= 1ULL << (Position & 63);
        }

        void write(uint64_t Position, bool Value)
        {
            if (Value)
                Storage[Position >> 6] |= 1ULL << (Position & 63);
            if (!Value)
                Storage[Position >> 6] &= ~(1ULL << (Position & 63));
        }

        bool read(uint64_t Position)
        {
            return Storage[Position >> 6] & (1ULL << (Position & 63));
        }

        void setValue(int Position, uint64_t Value)
        {
            Storage[Position] = Value;
        }

        // @Generics
        void resize(uint64_t MaskSize = i64)
        {
            delete[] Storage;

            uint64_t FinalSize = (MaskSize >> 6) + ((MaskSize & 63) > 0);
            Size = FinalSize;
            Storage = new uint64_t[FinalSize];
        }

        void clear()
        {
            delete[] Storage;
            Storage = new uint64_t[Size];
        }

        void free()
        {
            delete[] Storage;
            Storage = new uint64_t[1];
            Size = 1;
        }

        bool zero()
        {
            for (uint64_t Index = 0; Index < Size; Index++)
                if (Storage[Index] != 0)
                    return false;
            return true;
        }

        std::string concat()
        {
            std::stringstream Result;
            Result << "0x";
            for (uint64_t Index = Size - 1; Index > 0; Index--)
                Result << std::hex << Storage[Index] << "_";
            Result << std::hex << Storage[0] << "_";
            return Result.str();
        }

        uint64_t getValue(uint64_t Position)
        {
            return Storage[Position];
        }

        uint64_t getSize()
        {
            return Size;
        }

        DynamicBitset::b64 clone()
        {
            DynamicBitset::b64 Result(Size * i64);
            for (uint64_t Index = 0; Index < Size; Index++)
                Result.Storage[Index] = Storage[Index];
            return Result;
        }

        // @Constructor
        DynamicBitset::b64(uint64_t MaskSize = i64)
        {
            delete[] Storage;
            uint64_t FinalSize = (MaskSize >> 6) + ((MaskSize & 63) > 0);
            Size = FinalSize;
            Storage = new uint64_t[FinalSize];
        }

    };

    class b32 {

    private:

        uint32_t Size;
        uint32_t* Storage = new uint32_t[0];

    public:

        // @Overload

        bool operator==(DynamicBitset::b32& Target)
        {
            if (Target.getSize() != Size)
                return false;
            for (int Index = 0; Index < Size; Index++)
            {
                if (Target.getValue(Index) != Storage[Index])
                    return false;
            }
            return true;
        }

        bool operator!=(DynamicBitset::b32& Target)
        {
            if (Target.getSize() != Size)
                return true;
            for (int Index = 0; Index < Size; Index++)
            {
                if (Target.getValue(Index) != Storage[Index])
                    return true;
            }
            return false;
        }

        DynamicBitset::b32 operator|(DynamicBitset::b32& Target)
        {
            uint32_t TSize = Target.getSize();
            DynamicBitset::b32 Result(TSize * i32);
            for (uint32_t Index = 0; Index < TSize; Index++)
                Result.Storage[Index] = Storage[Index] | Target.getValue(Index);
            return Result;
        }

        DynamicBitset::b32 operator^(DynamicBitset::b32& Target)
        {
            uint32_t TSize = Target.getSize();
            DynamicBitset::b32 Result(TSize * i32);
            for (uint32_t Index = 0; Index < TSize; Index++)
                Result.Storage[Index] = Storage[Index] ^ Target.getValue(Index);
            return Result;
        }

        DynamicBitset::b32 operator&(DynamicBitset::b32& Target)
        {
            uint32_t TSize = Target.getSize();
            DynamicBitset::b32 Result(TSize * i32);
            for (uint32_t Index = 0; Index < TSize; Index++)
                Result.Storage[Index] = Storage[Index] & Target.getValue(Index);
            return Result;
        }

        DynamicBitset::b32 operator~()
        {
            DynamicBitset::b32 Result(Size * i32);
            for (uint32_t Index = 0; Index < Size; Index++)
                Result.Storage[Index] = mINT64_MAX ^ Storage[Index];
            return Result;
        }

        DynamicBitset::b32 operator<<(int Amount)
        {

            if (Amount < 0)
                return operator>>(-Amount);

            DynamicBitset::b32 Result(Size * i32);

            int ShiftAmount = Amount & 31;
            int Dif = 32 - ShiftAmount;
            Amount >>= 5;

            for (int Index = Size - 1; Index > -1; Index--)
                Result.Storage[Index] = (Index - Amount > -1) ? Storage[Index - Amount] : 0;

            for (int Index = Size - 1; Index > -1; Index--)
                Result.Storage[Index] = (Result.Storage[Index] << ShiftAmount) | ((Index - 1 > -1) ? Result.Storage[Index - 1] >> Dif : 0);

            return Result;
        }

        DynamicBitset::b32 operator>>(int Amount)
        {

            if (Amount < 0)
                return operator<<(-Amount);

            DynamicBitset::b32 Result(Size * i32);

            int ShiftAmount = Amount & 31;
            int Dif = 32 - ShiftAmount;
            Amount >>= 5;

            for (int Index = 0; Index < Size; Index++)
                Result.Storage[Index] = (Index + Amount < Size) ? Storage[Index + Amount] : 0;

            for (int Index = 0; Index < Size; Index++)
                Result.Storage[Index] = (Result.Storage[Index] >> ShiftAmount) | ((Index + 1 < Size) ? Result.Storage[Index + 1] << Dif : 0);

            return Result;
        }

        void operator<<=(int Amount)
        {

            if (Amount < 0)
                return operator>>=(-Amount);

            int ShiftAmount = Amount & 31;
            int Dif = 32 - ShiftAmount;
            Amount >>= 5;

            for (int Index = Size - 1; Index > -1; Index--)
                Storage[Index] = (Index - Amount > -1) ? Storage[Index - Amount] : 0;

            for (int Index = Size - 1; Index > -1; Index--)
                Storage[Index] = (Storage[Index] << ShiftAmount) | ((Index != 0) ? Storage[Index - 1] >> Dif : 0);

        }

        void operator>>=(int Amount)
        {

            if (Amount < 0)
                return operator<<=(-Amount);

            int ShiftAmount = Amount & 31;
            int Dif = 32 - ShiftAmount;
            Amount >>= 5;

            for (int Index = 0; Index < Size; Index++)
                Storage[Index] = (Index + Amount < Size) ? Storage[Index + Amount] : 0;

            for (int Index = 0; Index < Size; Index++)
                Storage[Index] = (Storage[Index] >> ShiftAmount) | ((Index + 1 < Size) ? Storage[Index + 1] << Dif : 0);
        }

        void operator|=(DynamicBitset::b32& Target)
        {
            uint32_t TSize = Target.getSize();
            for (uint32_t Index = 0; Index < TSize; Index++)
                Storage[Index] = Storage[Index] | Target.getValue(Index);
        }

        void operator^=(DynamicBitset::b32& Target)
        {
            uint32_t TSize = Target.getSize();
            for (uint32_t Index = 0; Index < TSize; Index++)
                Storage[Index] = Storage[Index] ^ Target.getValue(Index);
        }

        void operator&=(DynamicBitset::b32& Target)
        {
            uint32_t TSize = Target.getSize();
            for (uint32_t Index = 0; Index < TSize; Index++)
                Storage[Index] = Storage[Index] & Target.getValue(Index);
        }

        // @Read / Write
        void flip(uint32_t Position)
        {
            Storage[Position >> 5] ^= 1ULL << (Position & 31);
        }

        void write(uint32_t Position, bool Value)
        {
            if (Value)
                Storage[Position >> 5] |= 1ULL << (Position & 31);
            if (!Value)
                Storage[Position >> 5] &= ~(1ULL << (Position & 31));
        }

        bool read(uint32_t Position)
        {
            return Storage[Position >> 5] & (1ULL << (Position & 31));
        }

        void setValue(int Position, uint32_t Value)
        {
            Storage[Position] = Value;
        }

        // @Generics
        void resize(uint32_t MaskSize = i32)
        {
            delete[] Storage;

            uint32_t FinalSize = (MaskSize >> 5) + ((MaskSize & 31) > 0);
            Size = FinalSize;
            Storage = new uint32_t[FinalSize];
        }

        void clear()
        {
            delete[] Storage;
            Storage = new uint32_t[Size];
        }

        void free()
        {
            delete[] Storage;
            Storage = new uint32_t[1];
            Size = 1;
        }

        bool zero()
        {
            for (uint32_t Index = 0; Index < Size; Index++)
                if (Storage[Index] != 0)
                    return false;
            return true;
        }

        std::string concat()
        {
            std::stringstream Result;
            Result << "0x";
            for (uint32_t Index = Size - 1; Index > 0; Index--)
                Result << std::hex << Storage[Index] << "_";
            Result << std::hex << Storage[0] << "_";
            return Result.str();
        }

        uint32_t getValue(uint32_t Position)
        {
            return Storage[Position];
        }

        uint32_t getSize()
        {
            return Size;
        }

        DynamicBitset::b32 clone()
        {
            DynamicBitset::b32 Result(Size * i32);
            for (uint32_t Index = 0; Index < Size; Index++)
                Result.Storage[Index] = Storage[Index];
            return Result;
        }

        // @Constructor
        DynamicBitset::b32(uint32_t MaskSize = i32)
        {
            delete[] Storage;
            uint32_t FinalSize = (MaskSize >> 5) + ((MaskSize & 31) > 0);
            Size = FinalSize;
            Storage = new uint32_t[FinalSize];
        }

    };
};