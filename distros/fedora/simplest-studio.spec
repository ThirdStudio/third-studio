Summary:    Audio encoder
Name:       simplest-studio
Version:    1.1
Release:    1%{?dist}
License:    GPL-3
Group:      Applications/Audio
URL:        https://github.com/SimplestStudio/%{name}
Source0:    %{name}-%{version}.tar.xz

BuildRequires: gcc

Requires:   qt5-qtbase-common
Requires:   libmediainfo-devel
Requires:   ffmpeg>=4.2

# BuildArch:  noarch

%description
Simplest Studio is an application that allows you to convert audio files.

%prep
# nothing here

%setup -q
# nothing here

%build
%make_build -C app

%install
mkdir -p %{buildroot}
%make_install -C app INSTALL_ROOT=%{buildroot}

%post
# Setup icons
touch -c /usr/share/icons/hicolor
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
  gtk-update-icon-cache -tq /usr/share/icons/hicolor 2>/dev/null ||:
fi

# Setup desktop file
if command -v update-desktop-database >/dev/null 2>&1; then
  update-desktop-database -q /usr/share/applications 2>/dev/null ||:
fi

%postun
# nothing here

%clean
rm -rf %{buildroot}

%files
%doc ABOUT LICENSE
/opt/simplest_studio/bin/simplest_studio

%changelog
* Wed Sep 30 2020 Simplest Studio <depositmail@mail.ru>
- Initial package for Fedora.
