@echo on
:: example C\:/Users/l_chayka/AppData/Local/Android/Sdk/ndk/25.2.9519653/
set ndk=C\:/Users/l_chayka/AppData/Local/Android/Sdk/ndk/25.2.9519653
:: d:/boost
set prefix=d:/boost
set link_type=shared
set varian_type=release 

:: generate user-config.jam file in HOME directory (^ is escape char)

(
	echo using clang ^: arm64 ^:  %ndk%/toolchains/llvm/prebuilt/windows-x86_64/bin/aarch64-linux-android21-clang^+^+.cmd    ^: ^<cxxflags^>-std=c^+^+17 ;  
	echo using clang ^: arm ^:    %ndk%/toolchains/llvm/prebuilt/windows-x86_64/bin/armv7a-linux-androideabi21-clang^+^+.cmd ^: ^<cxxflags^>-std=c^+^+17 ;
	echo using clang ^: x86 ^:    %ndk%/toolchains/llvm/prebuilt/windows-x86_64/bin/i686-linux-android21-clang^+^+.cmd       ^: ^<cxxflags^>-std=c^+^+17 ;
	echo using clang ^: x86_64 ^: %ndk%/toolchains/llvm/prebuilt/windows-x86_64/bin/x86_64-linux-android21-clang^+^+.cmd     ^: ^<cxxflags^>-std=c^+^+17 ;
) > "%HOME%/user-config.jam"


:: build boost for every architecture
:: -q - stop on first error

b2 -q toolset=clang-x86_64 target-os=android link=%link_type% variant=%varian_type% debug-symbols=on threading=multi --layout=versioned --prefix=%prefix%-x64_86/ install
b2 -q toolset=clang-x86    target-os=android link=%link_type% variant=%varian_type% debug-symbols=on threading=multi --layout=versioned --prefix=%prefix%-x86/    install 
b2 -q toolset=clang-arm    target-os=android link=%link_type% variant=%varian_type% debug-symbols=on threading=multi --layout=versioned --prefix=%prefix%-arm/    install
b2 -q toolset=clang-arm64  target-os=android link=%link_type% variant=%varian_type% debug-symbols=on threading=multi --layout=versioned --prefix=%prefix%-arm64/  install
