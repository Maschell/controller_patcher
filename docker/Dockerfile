FROM devkitpro/devkitppc:latest

## Create & set workdir
RUN mkdir /opt/project &&\
    cd /opt/project
WORKDIR /opt/project

## Install dependencies

# Dynamic libs for WiiU homebrew.
COPY --from=wiiulegacy/dynamic_libs:0.1 /artifacts $DEVKITPRO/portlibs
COPY --from=wiiulegacy/libutils:0.1 /artifacts $DEVKITPRO/portlibs
