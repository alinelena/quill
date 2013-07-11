# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.26
# 

Name:       libquill-qt5

# >> macros
# << macros

Summary:    Qt based engine for mobile image processing applications
Version:    1.111202.0
Release:    0
Group:      System/Libraries
License:    LGPLv2
URL:        https://github.com/nemomobile/quill
Source0:    %{name}-%{version}.tar.gz
Source100:  libquill-qt5.yaml
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(quillimagefilter-qt5)
BuildRequires:  pkgconfig(quillmetadata-qt5)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(libexif)
BuildRequires:  pkgconfig(exempi-2.0)
BuildRequires:  fdupes

%description
A high-level, Qt-based engine for building image viewing and editing
applications especially for mobile devices. It provides optimized
image loading and handling, fast response times for previewing images,
memory-optimized handling of large images, transparent thumbnail
handling and non-destructive editing.


%package -n quill-qt5-utils
Summary:    Small independent utilities to support libquill use
Group:      Development/Tools
Requires:   %{name} = %{version}-%{release}

%description -n quill-qt5-utils
%{summary}.

%package tests
Summary:    Unit tests for libquill
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   testrunner-lite
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description tests
%{summary}.

%package devel
Summary:    Development headers for libquill
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
%qmake5
# << build pre


make %{?jobs:-j%jobs}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake5_install

# >> install post
# Remove duplicate pkgconfig file
rm %{buildroot}/%{_libdir}/quill.pc

# Remove development library for tests, it's not meant for use outside
# the quill unit tests
rm %{buildroot}/%{_libdir}/libunittests-quill.so
# << install post

%fdupes  %{buildroot}/%{_libdir}/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post tests -p /sbin/ldconfig

%postun tests -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
# >> files
%{_libdir}/%{name}*.so.*
# << files

%files -n quill-qt5-utils
%defattr(-,root,root,-)
# >> files quill-qt5-utils
%{_libdir}/quill-utils/quill-autoclean
# << files quill-qt5-utils

%files tests
%defattr(-,root,root,-)
# >> files tests
%{_libdir}/libunittests-quill*.so*
%{_libdir}/libquill-tests/*
%{_datadir}/libquill-tests/*
# << files tests

%files devel
%defattr(-,root,root,-)
# >> files devel
%{_includedir}/qt4/quill/
%{_libdir}/%{name}*.so
%{_libdir}/pkgconfig/*
%{_datadir}/qt4/mkspecs/features/quill.prf
# << files devel
