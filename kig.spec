Name: kig
Summary: Kig -- KDE Interactive Geometry
Version: 0.1
Release: 1
Copyright: GPL
Group: X11/KDE/Utilities
Source: ftp.kde.org/pub/kde/unstable/apps/utils/kig-0.1.tar.gz
Packager: Dominique Devriese <fritmebufstek@pandora.be>
BuildRoot: /tmp/kig-0.1
Prefix: /usr

%description
Kig provides interactive geometry on KDE.

%prep
rm -rf $RPM_BUILD_ROOT
%setup -n kig-0.1

%build
./configure --disable-debug --enable-final --prefix=%{prefix}
make

%install
make DESTDIR=$RPM_BUILD_ROOT install
find . -type f -o -type l | sed 's|^\.||' > $RPM_BUILD_ROOT/master.list

%clean
rm -rf $RPM_BUILD_ROOT

%files -f $RPM_BUILD_ROOT/master.list
