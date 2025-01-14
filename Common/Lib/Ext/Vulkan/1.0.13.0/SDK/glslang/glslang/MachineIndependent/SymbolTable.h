//
//Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
//Copyright (C) 2013 LunarG, Inc.
//
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _SYMBOL_TABLE_INCLUDED_
#define _SYMBOL_TABLE_INCLUDED_

//
// Symbol table for parsing.  Has these design characteristics:
//
// * Same symbol table can be used to compile many shaders, to preserve
//   effort of creating and loading with the large numbers of built-in
//   symbols.
//
// -->  This requires a copy mechanism, so initial pools used to create
//   the shared information can be popped.  Done through "clone"
//   methods.
//
// * Name mangling will be used to give each function a unique name
//   so that symbol table lookups are never ambiguous.  This allows
//   a simpler symbol table structure.
//
// * Pushing and popping of scope, so symbol table will really be a stack
//   of symbol tables.  Searched from the top, with new inserts going into
//   the top.
//
// * Constants:  Compile time constant symbols will keep their values
//   in the symbol table.  The parser can substitute constants at parse
//   time, including doing constant folding and constant propagation.
//
// * No temporaries:  Temporaries made from operations (+, --, .xy, etc.)
//   are tracked in the intermediate representation, not the symbol table.
//

#include "../Include/Common.h"
#include "../Include/intermediate.h"
#include "../Include/InfoSink.h"

namespace glslang {

//
// Symbol base class.  (Can build functions or variables out of these...)
//

class TVariable;
class TFunction;
class TAnonMember;

class TSymbol {
public:
    POOL_ALLOCATOR_NEW_DELETE(GetThreadPoolAllocator())
    explicit TSymbol(const TString *n) :  name(n), numExtensions(0), extensions(0), writable(true) { }
    virtual TSymbol* clone() const = 0;
    virtual ~TSymbol() { }  // rely on all symbol owned memory coming from the pool

    virtual const TString& getName() const { return *name; }
    virtual void changeName(const TString* newName) { name = newName; }
    virtual const TString& getMangledName() const { return getName(); }
    virtual TFunction* getAsFunction() { return 0; }
    virtual const TFunction* getAsFunction() const { return 0; }
    virtual TVariable* getAsVariable() { return 0; }
    virtual const TVariable* getAsVariable() const { return 0; }
    virtual const TAnonMember* getAsAnonMember() const { return 0; }
    virtual const TType& getType() const = 0;
    virtual TType& getWritableType() = 0;
    virtual void setUniqueId(int id) { uniqueId = id; }
    virtual int getUniqueId() const { return uniqueId; }
    virtual void setExtensions(int num, const char* const exts[])
    {
        assert(extensions == 0);
        assert(num > 0);
        numExtensions = num;
        extensions = NewPoolObject(exts[0], num);
        for (int e = 0; e < num; ++e)
            extensions[e] = exts[e];
    }
    virtual int getNumExtensions() const { return numExtensions; }
    virtual const char** getExtensions() const { return extensions; }
    virtual void dump(TInfoSink &infoSink) const = 0;

    virtual bool isReadOnly() const { return ! writable; }
    virtual void makeReadOnly() { writable = false; }

protected:
    explicit TSymbol(const TSymbol&);
    TSymbol& operator=(const TSymbol&);

    const TString *name;
    unsigned int uniqueId;      // For cross-scope comparing during code generation

    // For tracking what extensions must be present 
    // (don't use if correct version/profile is present).
    int numExtensions;
    const char** extensions; // an array of pointers to existing constant char strings

    //
    // N.B.: Non-const functions that will be generally used should assert on this,
    // to avoid overwriting shared symbol-table information.
    //
    bool writable;
};

//
// Variable class, meaning a symbol that's not a function.
//
// There could be a separate class hierarchy for Constant variables;
// Only one of int, bool, or float, (or none) is correct for
// any particular use, but it's easy to do this way, and doesn't
// seem worth having separate classes, and "getConst" can't simply return
// different values for different types polymorphically, so this is
// just simple and pragmatic.
//
class TVariable : public TSymbol {
public:
    TVariable(const TString *name, const TType& t, bool uT = false )
        : TSymbol(name), 
          userType(uT),
          constSubtree(nullptr) { type.shallowCopy(t); }
    virtual TVariable* clone() const;
    virtual ~TVariable() { }

    virtual TVariable* getAsVariable() { return this; }
    virtual const TVariable* getAsVariable() const { return this; }
    virtual const TType& getType() const { return type; }
    virtual TType& getWritableType() { assert(writable); return type; }
    virtual bool isUserType() const { return userType; }
    virtual const TConstUnionArray& getConstArray() const { return constArray; }
    virtual TConstUnionArray& getWritableConstArray() { assert(writable); return constArray; }
    virtual void setConstArray(const TConstUnionArray& array) { constArray = array; }
    virtual void setConstSubtree(TIntermTyped* subtree) { constSubtree = subtree; }
    virtual TIntermTyped* getConstSubtree() const { return constSubtree; }

    virtual void dump(TInfoSink &infoSink) const;

protected:
    explicit TVariable(const TVariable&);
    TVariable& operator=(const TVariable&);

    TType type;
    bool userType;
    // we are assuming that Pool Allocator will free the memory allocated to unionArray
    // when this object is destroyed

    // TODO: these two should be a union
    // A variable could be a compile-time constant, or a specialization
    // constant, or neither, but never both.
    TConstUnionArray constArray;  // for compile-time constant value
    TIntermTyped* constSubtree;   // for specialization constant computation
};

//
// The function sub-class of symbols and the parser will need to
// share this definition of a function parameter.
//
struct TParameter {
    TString *name;
    TType* type;
    void copyParam(const TParameter& param) 
    {
        if (param.name)
            name = NewPoolTString(param.name->c_str());
        else
            name = 0;
        type = param.type->clone();
    }
};

//
// The function sub-class of a symbol.
//
class TFunction : public TSymbol {
public:
    explicit TFunction(TOperator o) :
        TSymbol(0),
        op(o),
        defined(false), prototyped(false) { }
    TFunction(const TString *name, const TType& retType, TOperator tOp = EOpNull) :
        TSymbol(name),
        mangledName(*name + '('),
        op(tOp),
        defined(false), prototyped(false) { returnType.shallowCopy(retType); }
    virtual TFunction* clone() const;
    virtual ~TFunction();

    virtual TFunction* getAsFunction() { return this; }
    virtual const TFunction* getAsFunction() const { return this; }

    virtual void addParameter(TParameter& p)
    {
        assert(writable);
        parameters.push_back(p);
        p.type->appendMangledName(mangledName);
    }

    virtual const TString& getMangledName() const { return mangledName; }
    virtual const TType& getType() const { return returnType; }
    virtual TType& getWritableType() { return returnType; }
    virtual void relateToOperator(TOperator o) { assert(writable); op = o; }
    virtual TOperator getBuiltInOp() const { return op; }
    virtual void setDefined() { assert(writable); defined = true; }
    virtual bool isDefined() const { return defined; }
    virtual void setPrototyped() { assert(writable); prototyped = true; }
    virtual bool isPrototyped() const { return prototyped; }

    virtual int getParamCount() const { return static_cast<int>(parameters.size()); }
    virtual TParameter& operator[](int i) { assert(writable); return parameters[i]; }
    virtual const TParameter& operator[](int i) const { return parameters[i]; }

    virtual void dump(TInfoSink &infoSink) const;

protected:
    explicit TFunction(const TFunction&);
    TFunction& operator=(const TFunction&);

    typedef TVector<TParameter> TParamList;
    TParamList parameters;
    TType returnType;
    TString mangledName;
    TOperator op;
    bool defined;
    bool prototyped;
};

//
// Members of anonymous blocks are a kind of TSymbol.  They are not hidden in
// the symbol table behind a container; rather they are visible and point to
// their anonymous container.  (The anonymous container is found through the
// member, not the other way around.)
//
class TAnonMember : public TSymbol {
public:
    TAnonMember(const TString* n, unsigned int m, const TVariable& a, int an) : TSymbol(n), anonContainer(a), memberNumber(m), anonId(an) { }
    virtual TAnonMember* clone() const;
    virtual ~TAnonMember() { }

    virtual const TAnonMember* getAsAnonMember() const { return this; }
    virtual const TVariable& getAnonContainer() const { return anonContainer; }
    virtual unsigned int getMemberNumber() const { return memberNumber; }
    
    virtual const TType& getType() const
    {
        const TTypeList& types = *anonContainer.getType().getStruct();
        return *types[memberNumber].type;
    }

    virtual TType& getWritableType()
    {
        assert(writable);
        const TTypeList& types = *anonContainer.getType().getStruct();
        return *types[memberNumber].type;
    }
    
    virtual int getAnonId() const { return anonId; }
    virtual void dump(TInfoSink &infoSink) const;

protected:
    explicit TAnonMember(const TAnonMember&);
    TAnonMember& operator=(const TAnonMember&);

    const TVariable& anonContainer;
    unsigned int memberNumber;
    int anonId;
};

class TSymbolTableLevel {
public:
    POOL_ALLOCATOR_NEW_DELETE(GetThreadPoolAllocator())
    TSymbolTableLevel() : defaultPrecision(0), anonId(0) { }
    ~TSymbolTableLevel();

    bool insert(TSymbol& symbol, bool separateNameSpaces)
    {
        //
        // returning true means symbol was added to the table with no semantic errors
        //
        tInsertResult result;
        const TString& name = symbol.getName();
        if (name == "") {
            // An empty name means an anonymous container, exposing its members to the external scope.
            // Give it a name and insert its members in the symbol table, pointing to the container.
            char buf[20];
            snprintf(buf, 20, "%s%d", AnonymousPrefix, anonId);
            symbol.changeName(NewPoolTString(buf));

            bool isOkay = true;
            const TTypeList& types = *symbol.getAsVariable()->getType().getStruct();
            for (unsigned int m = 0; m < types.size(); ++m) {
                TAnonMember* member = new TAnonMember(&types[m].type->getFieldName(), m, *symbol.getAsVariable(), anonId);
                result = level.insert(tLevelPair(member->getMangledName(), member));
                if (! result.second)
                    isOkay = false;
            }

            ++anonId;

            return isOkay;
        } else {
            // Check for redefinition errors:
            // - STL itself will tell us if there is a direct name collision, with name mangling, at this level
            // - additionally, check for function-redefining-variable name collisions
            const TString& insertName = symbol.getMangledName();
            if (symbol.getAsFunction()) {
                // make sure there isn't a variable of this name
                if (! separateNameSpaces && level.find(name) != level.end())
                    return false;

                // insert, and whatever happens is okay
                level.insert(tLevelPair(insertName, &symbol));

                return true;
            } else {
                result = level.insert(tLevelPair(insertName, &symbol));

                return result.second;
            }
        }
    }

    TSymbol* find(const TString& name) const
    {
        tLevel::const_iterator it = level.find(name);
        if (it == level.end()) 
            return 0;
        else
            return (*it).second;
    }

    void findFunctionNameList(const TString& name, TVector<TFunction*>& list)
    {
        size_t parenAt = name.find_first_of('(');
        TString base(name, 0, parenAt + 1);

        tLevel::const_iterator begin = level.lower_bound(base);
        base[parenAt] = ')';  // assume ')' is lexically after '('
        tLevel::const_iterator end = level.upper_bound(base);
        for (tLevel::const_iterator it = begin; it != end; ++it)
            list.push_back(it->second->getAsFunction());
    }

    // See if there is already a function in the table having the given non-function-style name.
    bool hasFunctionName(const TString& name) const
    {
        tLevel::const_iterator candidate = level.lower_bound(name);
        if (candidate != level.end()) {
            const TString& candidateName = (*candidate).first;
            TString::size_type parenAt = candidateName.find_first_of('(');
            if (parenAt != candidateName.npos && candidateName.compare(0, parenAt, name) == 0)

                return true;
        }

        return false;
    }

    // See if there is a variable at this level having the given non-function-style name.
    // Return true if name is found, and set variable to true if the name was a variable.
    bool findFunctionVariableName(const TString& name, bool& variable) const
    {
        tLevel::const_iterator candidate = level.lower_bound(name);
        if (candidate != level.end()) {
            const TString& candidateName = (*candidate).first;
            TString::size_type parenAt = candidateName.find_first_of('(');
            if (parenAt == candidateName.npos) {
                // not a mangled name
                if (candidateName == name) {
                    // found a variable name match
                    variable = true;
                    return true;
                }
            } else {
                // a mangled name
                if (candidateName.compare(0, parenAt, name) == 0) {
                    // found a function name match
                    variable = false;
                    return true;
                }
            }
        }

        return false;
    }

    // Use this to do a lazy 'push' of precision defaults the first time
    // a precision statement is seen in a new scope.  Leave it at 0 for
    // when no push was needed.  Thus, it is not the current defaults,
    // it is what to restore the defaults to when popping a level.
    void setPreviousDefaultPrecisions(const TPrecisionQualifier *p)
    {
        // can call multiple times at one scope, will only latch on first call,
        // as we're tracking the previous scope's values, not the current values
        if (defaultPrecision != 0)
            return;

        defaultPrecision = new TPrecisionQualifier[EbtNumTypes];
        for (int t = 0; t < EbtNumTypes; ++t)
            defaultPrecision[t] = p[t];
    }

    void getPreviousDefaultPrecisions(TPrecisionQualifier *p)
    {
        // can be called for table level pops that didn't set the
        // defaults
        if (defaultPrecision == 0 || p == 0)
            return;

        for (int t = 0; t < EbtNumTypes; ++t)
            p[t] = defaultPrecision[t];
    }

    void relateToOperator(const char* name, TOperator op);
    void setFunctionExtensions(const char* name, int num, const char* const extensions[]);
    void dump(TInfoSink &infoSink) const;
    TSymbolTableLevel* clone() const;
    void readOnly();

protected:
    explicit TSymbolTableLevel(TSymbolTableLevel&);
    TSymbolTableLevel& operator=(TSymbolTableLevel&);

    typedef std::map<TString, TSymbol*, std::less<TString>, pool_allocator<std::pair<const TString, TSymbol*> > > tLevel;
    typedef const tLevel::value_type tLevelPair;
    typedef std::pair<tLevel::iterator, bool> tInsertResult;

    tLevel level;  // named mappings
    TPrecisionQualifier *defaultPrecision;
    int anonId;
};

class TSymbolTable {
public:
    TSymbolTable() : uniqueId(0), noBuiltInRedeclarations(false), separateNameSpaces(false), adoptedLevels(0)
    {
        //
        // This symbol table cannot be used until push() is called.
        //
    }
    ~TSymbolTable()
    {
        // this can be called explicitly; safest to code it so it can be called multiple times

        // don't deallocate levels passed in from elsewhere
        while (table.size() > adoptedLevels)
            pop(0);
    }
    
    void adoptLevels(TSymbolTable& symTable)
    {
        for (unsigned int level = 0; level < symTable.table.size(); ++level) {
            table.push_back(symTable.table[level]);
            ++adoptedLevels;
        }
        uniqueId = symTable.uniqueId;
        noBuiltInRedeclarations = symTable.noBuiltInRedeclarations;
        separateNameSpaces = symTable.separateNameSpaces;
    }

    //
    // While level adopting is generic, the methods below enact a the following
    // convention for levels:
    //   0: common built-ins shared across all stages, all compiles, only one copy for all symbol tables
    //   1: per-stage built-ins, shared across all compiles, but a different copy per stage
    //   2: built-ins specific to a compile, like resources that are context-dependent, or redeclared built-ins
    //   3: user-shader globals
    //
protected:
    static const int globalLevel = 3;
    bool isSharedLevel(int level)  { return level <= 1; }              // exclude all per-compile levels
    bool isBuiltInLevel(int level) { return level <= 2; }              // exclude user globals
    bool isGlobalLevel(int level)  { return level <= globalLevel; }    // include user globals
public:
    bool isEmpty() { return table.size() == 0; }
    bool atBuiltInLevel() { return isBuiltInLevel(currentLevel()); }
    bool atGlobalLevel()  { return isGlobalLevel(currentLevel()); }

    void setNoBuiltInRedeclarations() { noBuiltInRedeclarations = true; }
    void setSeparateNameSpaces() { separateNameSpaces = true; }
    
    void push()
    {
        table.push_back(new TSymbolTableLevel);
    }

    void pop(TPrecisionQualifier *p)
    {
        table[currentLevel()]->getPreviousDefaultPrecisions(p);
        delete table.back();
        table.pop_back();
    }

    //
    // Insert a visible symbol into the symbol table so it can
    // be found later by name.
    //
    // Returns false if the was a name collision.
    //
    bool insert(TSymbol& symbol)
    {
        symbol.setUniqueId(++uniqueId);

        // make sure there isn't a function of this variable name
        if (! separateNameSpaces && ! symbol.getAsFunction() && table[currentLevel()]->hasFunctionName(symbol.getName()))
            return false;
            
        // check for not overloading or redefining a built-in function
        if (noBuiltInRedeclarations) {
            if (atGlobalLevel() && currentLevel() > 0) {
                if (table[0]->hasFunctionName(symbol.getName()))
                    return false;
                if (currentLevel() > 1 && table[1]->hasFunctionName(symbol.getName()))
                    return false;
            }
        }

        return table[currentLevel()]->insert(symbol, separateNameSpaces);
    }

    //
    // To allocate an internal temporary, which will need to be uniquely
    // identified by the consumer of the AST, but never need to 
    // found by doing a symbol table search by name, hence allowed an
    // arbitrary name in the symbol with no worry of collision.
    //
    void makeInternalVariable(TSymbol& symbol)
    {
        symbol.setUniqueId(++uniqueId);
    }

    //
    // Copy a variable or anonymous member's structure from a shared level so that
    // it can be added (soon after return) to the symbol table where it can be
    // modified without impacting other users of the shared table.
    //
    TSymbol* copyUpDeferredInsert(TSymbol* shared)
    {
        if (shared->getAsVariable()) {
            TSymbol* copy = shared->clone();
            copy->setUniqueId(shared->getUniqueId());
            return copy;
        } else {
            const TAnonMember* anon = shared->getAsAnonMember();
            assert(anon);
            TVariable* container = anon->getAnonContainer().clone();
            container->changeName(NewPoolTString(""));
            container->setUniqueId(anon->getAnonContainer().getUniqueId());
            return container;
        }
    }

    TSymbol* copyUp(TSymbol* shared)
    {
        TSymbol* copy = copyUpDeferredInsert(shared);
        table[globalLevel]->insert(*copy, separateNameSpaces);
        if (shared->getAsVariable())
            return copy;
        else {
            // return the copy of the anonymous member
            return table[globalLevel]->find(shared->getName());
        }
    }

    TSymbol* find(const TString& name, bool* builtIn = 0, bool *currentScope = 0)
    {
        int level = currentLevel();
        TSymbol* symbol;
        do {
            symbol = table[level]->find(name);
            --level;
        } while (symbol == 0 && level >= 0);
        level++;
        if (builtIn)
            *builtIn = isBuiltInLevel(level);
        if (currentScope)
            *currentScope = isGlobalLevel(currentLevel()) || level == currentLevel();  // consider shared levels as "current scope" WRT user globals

        return symbol;
    }

    bool isFunctionNameVariable(const TString& name) const
    {
        if (separateNameSpaces)
            return false;

        int level = currentLevel();
        do {
            bool variable;
            bool found = table[level]->findFunctionVariableName(name, variable);
            if (found)
                return variable;
            --level;
        } while (level >= 0);

        return false;
    }

    void findFunctionNameList(const TString& name, TVector<TFunction*>& list, bool& builtIn)
    {
        // For user levels, return the set found in the first scope with a match
        builtIn = false;
        int level = currentLevel();
        do {
            table[level]->findFunctionNameList(name, list);
            --level;
        } while (list.empty() && level >= globalLevel);

        if (! list.empty())
            return;

        // Gather across all built-in levels; they don't hide each other
        builtIn = true;
        do {
            table[level]->findFunctionNameList(name, list);
            --level;
        } while (level >= 0);
    }

    void relateToOperator(const char* name, TOperator op)
    {
        for (unsigned int level = 0; level < table.size(); ++level)
            table[level]->relateToOperator(name, op);
    }
    
    void setFunctionExtensions(const char* name, int num, const char* const extensions[])
    {
        for (unsigned int level = 0; level < table.size(); ++level)
            table[level]->setFunctionExtensions(name, num, extensions);
    }

    void setVariableExtensions(const char* name, int num, const char* const extensions[])
    {
        TSymbol* symbol = find(TString(name));
        if (symbol)
            symbol->setExtensions(num, extensions);
    }

    int getMaxSymbolId() { return uniqueId; }
    void dump(TInfoSink &infoSink) const;
    void copyTable(const TSymbolTable& copyOf);

    void setPreviousDefaultPrecisions(TPrecisionQualifier *p) { table[currentLevel()]->setPreviousDefaultPrecisions(p); }

    void readOnly()
    {
        for (unsigned int level = 0; level < table.size(); ++level)
            table[level]->readOnly();
    }

protected:
    TSymbolTable(TSymbolTable&);
    TSymbolTable& operator=(TSymbolTableLevel&);

    int currentLevel() const { return static_cast<int>(table.size()) - 1; }

    std::vector<TSymbolTableLevel*> table;
    int uniqueId;     // for unique identification in code generation
    bool noBuiltInRedeclarations;
    bool separateNameSpaces;
    unsigned int adoptedLevels;
};

} // end namespace glslang

#endif // _SYMBOL_TABLE_INCLUDED_
