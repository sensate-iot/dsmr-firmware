#pragma once

#include <Arduino.h>

namespace sensateiot
{
    static constexpr char kEncodeLookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static constexpr char kPadCharacter = '=';

    static String base64Encode(const String& input)
    {
        String encoded;
        encoded.reserve(((input.length() / 3) + (input.length() % 3 > 0)) * 4);

        std::uint32_t temp{};
        auto it = input.begin();

        for(std::size_t i = 0; i < input.length() / 3; ++i)
        {
            temp  = (*it++) << 16;
            temp += (*it++) << 8;
            temp += (*it++);

            encoded.concat(kEncodeLookup[(temp & 0x00FC0000) >> 18]);
            encoded.concat(kEncodeLookup[(temp & 0x0003F000) >> 12]);
            encoded.concat(kEncodeLookup[(temp & 0x00000FC0) >> 6 ]);
            encoded.concat(kEncodeLookup[(temp & 0x0000003F)      ]);
        }

        switch(input.length() % 3)
        {
        case 1:
            temp = (*it++) << 16;
            encoded.concat(kEncodeLookup[(temp & 0x00FC0000) >> 18]);
            encoded.concat(kEncodeLookup[(temp & 0x0003F000) >> 12]);

            encoded.concat(kPadCharacter);
            encoded.concat(kPadCharacter);
            break;

        case 2:
            temp  = (*it++) << 16;
            temp += (*it++) << 8;

            encoded.concat(kEncodeLookup[(temp & 0x00FC0000) >> 18]);
            encoded.concat(kEncodeLookup[(temp & 0x0003F000) >> 12]);
            encoded.concat(kEncodeLookup[(temp & 0x00000FC0) >> 6 ]);
            encoded.concat(kPadCharacter);
            break;
        }

        return encoded;
    }
} // namespace sensateiot
