#ifndef  METARECORD_VECTOR_H
# define METARECORD_VECTOR_H

# include <QVector>
# include "metarecordable.h"

template<typename VALUE_TYPE, typename SOURCE>
QVector<VALUE_TYPE*> model_vector_cast(const SOURCE& source)
{
  QVector<VALUE_TYPE*> result;
  VALUE_TYPE* value;

  result.reserve(source.size());
  for (MetaRecordable* model : source)
  {
    value = dynamic_cast<VALUE_TYPE*>(model);
    if (value)
      result.push_back(value);
  }
  return result;
}

template<typename LIST>
LIST select(const LIST& source, std::function<bool (typename LIST::value_type)> condition)
{
  LIST list;

  for (auto it = source.begin() ; it != source.end() ; ++it)
  {
    if (condition(*it))
      list.push_back(*it);
  }
  return list;
}

#endif
