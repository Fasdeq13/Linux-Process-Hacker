# Maintainer: Fasdeq13
pkgname=linux-process-hacker
pkgver=0.2
pkgrel=1
pkgdesc="Native C++/Qt6 system & process monitor for Linux with rootkit detection, I/O monitor, memory dumper and autostart manager"
arch=('x86_64')
url="https://github.com/Fasdeq13/Linux-Process-Hacker"
license=('MIT')
depends=('qt6-base' 'hicolor-icon-theme')
makedepends=('cmake' 'ninja' 'gcc')
source=("$pkgname-$pkgver.tar.gz::https://github.com/Fasdeq13/Linux-Process-Hacker/archive/refs/tags/Release-$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cmake -B build -S "Linux-Process-Hacker-Release-$pkgver" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build
}

package() {
    DESTDIR="$pkgdir" cmake --install build
}
