#include "album.h"


Album::Album(const QString& name):
   mId(-1), mName{name}
{
}

int Album::id() const {return Album::mId;}
void Album::setId(int id) {Album::mId = id;}
QString Album::name() const {return mName;}
void Album::setName(const QString &name) {mName = name;}
