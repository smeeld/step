Name:	ball	
Version: 1.0	
Release:	1%{?dist}
Summary: Balls	

Group:	Application	
License:	GPL
URL:		www.rockfm.ru
Source0:	ball.tar.bz2
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	libjpeg-turbo-devel >= 1.2.1 libjpeg-devel >= 1.2.1 mesa-libGL-devel >= 9.2 gcc >= 4.4.7
Requires:	libjpeg-turbo >= 1.2.1 libjpeg >= 1.2.1 mesa-libGL >= 9.2

%description
Balls

%prep
%setup -q

%build

make %{?_smp_mflags}


%install
rm -rf %{buildroot}
echo "Builroor is %{buildroot}"
make install DESTDIR=%{buildroot}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
/usr/share/prog
/usr/share/prog/models
/usr/share/prog/models/sphere.txt
/usr/share/prog/models/sphere2.txt
/usr/share/prog/models/sphere3.txt
/usr/share/prog/models/sphere4.txt
/usr/share/prog/models/sphere5.txt
/usr/share/prog/models/scene.txt
/usr/share/prog/models/fone.txt
/usr/share/prog/image
/usr/share/prog/image/s.jpg
/usr/share/prog/image/s2.jpg
/usr/share/prog/image/s3.jpg
/usr/share/prog/image/s1.jpg
/usr/share/prog/image/s.jpg
/usr/share/prog/shaders
/usr/share/prog/shaders/vs.glsl
/usr/share/prog/shaders/fs.glsl
/usr/share/prog/shaders/fone_fs.glsl
/usr/share/prog/shaders/fone_vs.glsl
/usr/bin/ball
%dir
/usr/share/prog
/usr/share/prog/image
/usr/share/prog/models
/usr/share/prog/shaders



%changelog

