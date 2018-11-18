/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2009 Matus Tomlein

 Synkron is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public Licence
 as published by the Free Software Foundation; either version 2
 of the Licence, or (at your option) any later version.

 Synkron is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public Licence for more details.

 You should have received a copy of the GNU General Public Licence
 along with Synkron; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
********************************************************************/

#include <QStringList>

class MTDictionary
{
public:
    MTDictionary() { allow_duplicate_keys = false; };
    MTDictionary(const QString & key, const QString & value) {
        allow_duplicate_keys = false;
        dict_keys << key; dict_values << value;
    };
    MTDictionary(bool allow_duplicate_keys) { this->allow_duplicate_keys = allow_duplicate_keys; };
    MTDictionary(const MTDictionary & other) {
        allow_duplicate_keys = other.allow_duplicate_keys;
        dict_keys = other.dict_keys;
        dict_values = other.dict_values;
    };

    void allowDuplicateKeys() { allow_duplicate_keys = true; };
    int count() const { return dict_keys.count(); };
    bool isEmpty() const { return dict_keys.isEmpty(); };
    void insert(const QString & key, const QString & value) {
        while (dict_keys.contains(key) && !allow_duplicate_keys) {
            int i = dict_keys.indexOf(key);
            dict_keys.removeAt(i);
            dict_values.removeAt(i);
        }
        dict_keys << key; dict_values << value;
    };
    void insertByValue(const QString & key, const QString & value) { // Inserts the value unless it is already contained
        for (int i = 0; i < count(); ++i) {
            if (this->key(i) == key && this->value(i) == value) return;
        }
        dict_keys << key; dict_values << value;
    };
    void setValue(const QString & key, const QString & value) {
        if (contains(key)) { dict_values.replace(indexOfKey(key), value); }
        else { dict_keys << key; dict_values << value; }
    };
    QString key(int i) const { if (i >= 0 && i < dict_keys.count()) return dict_keys.at(i); else return QString(); };
    QString firstKey(const QString & value) const {
        return dict_values.indexOf(value) < 0 ? value : dict_keys.at(dict_values.indexOf(value));
    };
    QStringList keys() const { return dict_keys; };
    int indexOfKey(const QString & key) const { return dict_keys.indexOf(key); };
    bool contains(const QString & key) const {
        return dict_keys.contains(key, Qt::CaseSensitive);
    };
    QString value(int i) const { if (i >= 0 && i < dict_keys.count()) return dict_values.at(i); else return QString(); };
    QString value(const QString & key) const {
        return dict_keys.indexOf(key) < 0 ? key : dict_values.at(dict_keys.indexOf(key));
    };
    QString value(const QString & key, const QString & default_value) const {
        return dict_keys.indexOf(key) < 0 ? default_value : dict_values.at(dict_keys.indexOf(key));
    };
    QStringList values() const { return dict_values; };
    int indexOfValue(const QString & value, int from = 0) const { return dict_values.indexOf(value, from); };
    int lastIndexOfValue(const QString & value, int from = -1) const { return dict_values.lastIndexOf(value, from); };
    void removeAt(int i) { if (i >= 0 && i < count()) { dict_keys.removeAt(i); dict_values.removeAt(i); } };
    void remove(const QString & key) {
        while (dict_keys.contains(key)) {
            int i = dict_keys.indexOf(key);
            dict_keys.removeAt(i);
            dict_values.removeAt(i);
        }
    };
    void clear() { dict_keys.clear(); dict_values.clear(); };
    MTDictionary & operator=(const MTDictionary & other) {
        allow_duplicate_keys = other.allow_duplicate_keys;
        dict_keys = other.dict_keys;
        dict_values = other.dict_values;
        return *this;
    };
    MTDictionary & operator<<(const MTDictionary & other) {
        for (int i = 0; i < other.count(); ++i) {
            if (contains(other.key(i))) { dict_values.replace(indexOfKey(other.key(i)), other.value(i)); }
            else { dict_keys << other.key(i); dict_values << other.value(i); }
        }
        return *this;
    };

private:
    bool allow_duplicate_keys;
    QStringList dict_keys;
    QStringList dict_values;
};

template<class Key, class T>
class MTMap
{
public:
    MTMap() { allow_duplicate_keys = false; };
    MTMap(const Key & key, const T & value) {
        allow_duplicate_keys = false;
        dict_keys << key; dict_values << value;
    };
    MTMap(bool allow_duplicate_keys) { this->allow_duplicate_keys = allow_duplicate_keys; };
    MTMap(const MTMap & other) {
        allow_duplicate_keys = other.allow_duplicate_keys;
        dict_keys = other.dict_keys;
        dict_values = other.dict_values;
    };

    void allowDuplicateKeys() { allow_duplicate_keys = true; };
    int count() const { return dict_keys.count(); };
    bool isEmpty() const { return dict_keys.isEmpty(); };
    void insert(const Key & key, const T & value) {
        while (dict_keys.contains(key) && !allow_duplicate_keys) {
            int i = dict_keys.indexOf(key);
            dict_keys.removeAt(i);
            dict_values.removeAt(i);
        }
        dict_keys << key; dict_values << value;
    };
    void insertByValue(const Key & key, const T & value) { // Inserts the value unless it is already contained
        for (int i = 0; i < count(); ++i) {
            if (this->key(i) == key && this->value(i) == value) return;
        }
        dict_keys << key; dict_values << value;
    };
    void setValue(const Key & key, const T & value) {
        if (contains(key)) { dict_values.replace(indexOfKey(key), value); }
        else { dict_keys << key; dict_values << value; }
    };
    void prependValue(const Key & key, const T & value) {
        if (contains(key)) { dict_values.replace(indexOfKey(key), value); }
        else { dict_keys.prepend(key); dict_values.prepend(value); }
    };
    void appendValue(const Key & key, const T & value) {
        if (contains(key)) { dict_values.replace(indexOfKey(key), value); }
        else { dict_keys.append(key); dict_values.append(value); }
    };
    Key key(int i) const { if (i >= 0 && i < dict_keys.count()) return dict_keys.at(i); else return Key(); };
    Key firstKey(const T & value) const {
        return dict_values.indexOf(value) < 0 ? value : dict_keys.at(dict_values.indexOf(value));
    };
    QList<Key> keys() const { return dict_keys; };
    int indexOfKey(const Key & key) const { return dict_keys.indexOf(key); };
    bool contains(const Key & key) const {
        return dict_keys.contains(key);
    };
    T value(int i) const { if (i >= 0 && i < dict_keys.count()) return dict_values.at(i); else return T(); };
    T value(const Key & key) const {
        return dict_keys.indexOf(key) < 0 ? key : dict_values.at(dict_keys.indexOf(key));
    };
    T value(const Key & key, const T & default_value) const {
        return dict_keys.indexOf(key) < 0 ? default_value : dict_values.at(dict_keys.indexOf(key));
    };
    QList<T> values() const { return dict_values; };
    int indexOfValue(const T & value, int from = 0) const { return dict_values.indexOf(value, from); };
    int lastIndexOfValue(const T & value, int from = -1) const { return dict_values.lastIndexOf(value, from); };
    void removeAt(int i) { if (i >= 0 && i < count()) { dict_keys.removeAt(i); dict_values.removeAt(i); } };
    void remove(const Key & key) {
        while (dict_keys.contains(key)) {
            int i = dict_keys.indexOf(key);
            dict_keys.removeAt(i);
            dict_values.removeAt(i);
        }
    };
    void clear() { dict_keys.clear(); dict_values.clear(); };
    MTMap & operator=(const MTMap & other) {
        allow_duplicate_keys = other.allow_duplicate_keys;
        dict_keys = other.dict_keys;
        dict_values = other.dict_values;
        return *this;
    };
    MTMap & operator<<(const MTMap & other) {
        for (int i = 0; i < other.count(); ++i) {
            if (contains(other.key(i))) { dict_values.replace(indexOfKey(other.key(i)), other.value(i)); }
            else { dict_keys << other.key(i); dict_values << other.value(i); }
        }
        return *this;
    };

private:
    bool allow_duplicate_keys;
    QList<Key> dict_keys;
    QList<T> dict_values;
};
