#ifndef FILTERBASETYPE_H
#define FILTERBASETYPE_H

#include <QtCore/QString>

namespace ICEODB_NS
{

class FilterBaseType
{
public:
	FilterBaseType();
	~FilterBaseType();

	const QString& getTypeName() const;

protected:
	QString m_typeName;
};

} // namespace ICEODB_NS

#endif // FILTERBASETYPE_H
