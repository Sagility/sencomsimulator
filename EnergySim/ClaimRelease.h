// ClaimRelease.h

//#ifdef NNNNNN

#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include "config.h"
#include "Job.h"
#include "SimEngineTimer.h"
#include "TimeValues.h"
#include <list>

using namespace std;

namespace EnergySim {
	#pragma region Forward_declarations
	class SimEngineTimer;
	class SimContext;
	template <typename elemType> class ClaimByClaimerJob;
	template <typename elemType> class ClaimJob;
	template <typename elemType> class ReleaseJob;
	#pragma endregion Forward_declarations

	class ProcessRequirmement
	{
		list<pair<int, double>> resTimeList = list<pair<int, double>>();
	};

	template <typename T>
	class ClaimReleaseFactory
	{
	public:
		ClaimReleaseFactory(SimContext* thectx)
		{
			itsctx = thectx;
		}
		ClaimJob<T>* getClaimJob(T theT)
		{
			return new ClaimJob<T>(itsctx, theT);
		}
		ReleaseJob<T>* getReleaseJob(T theT)
		{
			return new ReleaseJob<T>(itsctx, theT);
		}

	private:
		SimContext *itsctx;
	};
	template <typename T>
	class  IClaimer
	{
	private:
		string _id;
	public: 
		IClaimer(string id){_id=id;}
		string ID() const { return _id; }
		virtual void AddClaimedValue(ClaimByClaimerJob<T> *job)=0;
		virtual void ReleaseClaimedValue(T key)=0;
		virtual void ReleaseAllClaimedValues()=0;
		virtual void ReleaseAllClaimedValuesExcept(T key)=0;
		virtual ClaimByClaimerJob<T> ClaimValue(T theKey)=0;
	};

	template <typename T>  
	class  ClaimReleaseResourceHandler
	{
	protected:
		static ClaimReleaseResourceHandler<T>* _instance;
		ClaimReleaseResourceHandler(){
			usetestscenarios=false;
			defaultcapacity=1;
		} 
		map< T, list<ClaimJob<T>*>> _claimers;
		bool usetestscenarios;
		map< T, int> _capacities;
		int defaultcapacity;
		SimContext *_ctx;
		ClaimJob<T>* GetNextWaitingClaimer(T val);

	public:
		static ClaimReleaseResourceHandler* instance();
		bool use_release_without_exception() const { return usetestscenarios;}
		void set_use_release_without_exception(bool val){usetestscenarios=val;}
		SimContext *context()const{return _ctx;}
		void set_context(SimContext *ctx){_ctx=ctx;}

		bool ClaimValue(T val, ClaimJob<T>* claimer);
		void SetCapacity(T val, int capacity);
		unsigned int GetCapacity(T val) const;
		unsigned int AmountOfClaimers(T val);
		bool IsValueClaimed(T val);
		void ReleaseValueNoExceptionIfNOK(T val);
		string Diagnostics();
		void ReleaseValue(T val);
		/// <summary>
		/// we assume here that values are released FIFO
		/// </summary>
		/// <param name="val"></param>
		void RemoveClaimer(ClaimJob<T> *job);

		void ResetAll();

	};

	template <typename T>
	class  ClaimJob: public IPreemptableJob
	{
	protected: 
		T _key;
	public:
		//constructor
		ClaimJob(SimContext *context, T thekey):IPreemptableJob(context)
		{
			_key=thekey;
		}
		virtual ~ClaimJob()
		{
			// clear all lists
		}
		T key() const{return _key;};

		virtual void Execute()
		{
			Start();
			Claim();
		}
		virtual void Claim();
		virtual void Preempt()
		{
			NotifyJobPreempted();
			// timer.Preempt();
			NotifyJobFinished();
		}
		virtual string ToString()
		{
			std::ostringstream strs;
			strs << _key;
			std::string str = strs.str();
			return "ClaimJob claiming " + str;
		} 
		virtual string classname() { return "ClaimJob";}
	};

	template <typename T>
	class  ClaimByClaimerJob : public ClaimJob<T>
	{
	protected: IClaimer<T>* _claimer;
	public:
		ClaimByClaimerJob(SimContext *ctx, T theKey, IClaimer<T>* ld)
			: ClaimJob(ctx, theKey)
		{
			_claimer = ld;
		}
		IClaimer<T>* claimer() const{return _claimer;}
		virtual void Claim();

		virtual string ToString()
		{
			if (_claimer == NULL) return "ClaimByClaimerJob Empty";
			std::ostringstream strs;
			strs << "ClaimByClaimerJob " << _claimer->ID() << " claiming " << _key;
			std::string str = strs.str();
			return str;
		}
		virtual string classname() { return "ClaimByClaimerJob";}

	};
	template <typename T>
	class  ReleaseJob : public IJob
	{
	private:
		T _key;
		ClaimByClaimerJob<T> *_job;

	public:
		ReleaseJob(SimContext *ctx,T theKey):IJob(ctx)
		{
			_key = theKey;
			_job = NULL;
		}

		ReleaseJob(SimContext *ctx,ClaimByClaimerJob<T> *theClaimer):IJob(ctx)
		{
			if (theClaimer != NULL)
			{
				_key = theClaimer->key();
				_job = theClaimer;
			}
			else throw std::invalid_argument("ReleaseJob called with NULL Claimer") ;
		}
		virtual void Execute();
	};
	
	

	template <typename T> void ClaimJob<T>::Claim()
	{
		if (ClaimReleaseResourceHandler<T>::instance()->ClaimValue(_key,this))
		{
			Finish();
		}
	}
	template <typename T> void ClaimByClaimerJob<T>::Claim()		
	{
		if (ClaimReleaseResourceHandler<T>::instance()->ClaimValue(_key, this))
		{
			if(_ctx->environment()!=NULL){
				std::ostringstream strs;
				strs << "Claimer " << _claimer->ID() << " claiming " << _key;
				std::string str = strs.str();
				_ctx->environment()->DebugLog(str);
			}
			_claimer->AddClaimedValue(this);
			Finish();
		}

	}
	template <typename T> void ReleaseJob<T>::Execute(){
		Start();
		try
		{
			if(_job==NULL)ClaimReleaseResourceHandler<T>::instance()->ReleaseValue(_key);
			else ClaimReleaseResourceHandler<T>::instance()->RemoveClaimer(_job);
		}
		catch (std::invalid_argument iaex)
		{
			if(_ctx->environment()!=NULL){
				std::ostringstream strs;
				strs << "ReleaseJob Execute caught invalid argument exception: " << iaex.what();
				std::string str = strs.str();
				_ctx->environment()->ErrorLog(str);
			}
		}
		Finish();

	}
	template <typename T>  ClaimReleaseResourceHandler<T>* ClaimReleaseResourceHandler<T>::_instance = NULL;
	template <typename T>  ClaimReleaseResourceHandler<T>* ClaimReleaseResourceHandler<T>::instance(){
		if(_instance==NULL)_instance= new ClaimReleaseResourceHandler<T>();
		return _instance;
	}
	template <typename T>  unsigned int  ClaimReleaseResourceHandler<T>::GetCapacity(T val)const
	{
		unsigned int res = 1;
		if(_capacities.count(val)>0){
			return _capacities.at(val);
		}
		return defaultcapacity;
	}
	template <typename T>  void ClaimReleaseResourceHandler<T>::SetCapacity(T val, int capacity){
		// for now only do this when there are no claimers waiting
		if(_claimers.count(val) <1 ){
			_capacities.at(val) = capacity;
		}
	}
	template <typename T> unsigned int ClaimReleaseResourceHandler<T>::AmountOfClaimers(T val)
	{
		if(_claimers.count(val) <1) return 0;
		return _claimers.at(val).size();
	}
	template <typename T> void ClaimReleaseResourceHandler<T>::ReleaseValueNoExceptionIfNOK(T val)
	{
		if (use_release_without_exception())
		{
			if(_ctx->environment()!=NULL){
				std::ostringstream strs;
				strs << "ReleaseValueNoExceptionIfNOK " << val << endl;
				std::string str = strs.str();
				_ctx->environment()->DebugLog(str);
			}
			if(_claimers.count(val)<1) return;
			ReleaseValue(val);
		}
	}
	template <typename T> string ClaimReleaseResourceHandler<T>::Diagnostics()	   
	{
		try
		{
			std::ostringstream result; 

			std::ostringstream line;
			for (std::map<long, list<ClaimJob*>>::iterator it = _claimers.begin(); it!=_claimers.end(); ++it)
			{
				list<ClaimJob*> l =it->second;
				unsigned int cap = GetCapacity(it->first);
				unsigned int i=0;
				if(l.size()>0){
					for(std::list<ClaimJob *>::iterator cjid = l.begin(); cjid != l.end(); ++cjid){
						if (i < cap) line << it->first << "\tclaimed\t\n";
						else line << it->first << "\twaiting for claimer:\t\n";
						result << line;
						i++;
					}
				}
			}
			return result.str();

		}
		catch (std::exception ex)
		{
			if(_ctx->environment()!=NULL){
				std::ostringstream strs;
				strs << "ClaimReleaseResourceHandler Diagnostics caught exception: " << ex.what() << endl;
				std::string str = strs.str();
				_ctx->environment()->ErrorLog(str);			
			}
			return "";
		}
	}
	template <typename T> bool ClaimReleaseResourceHandler<T>::IsValueClaimed(T val)
	{
		if(_claimers.count(val) <1) return false;
		if(_claimers.at(val).size()>0) return true;
		return false;
	}
	template <typename T> bool ClaimReleaseResourceHandler<T>::ClaimValue(T val, ClaimJob<T>* claimer){
		if(_ctx->environment()!=NULL){
			std::ostringstream strs;
			strs << "Trying to ClaimValue " << val;
			std::string str = strs.str();
			_ctx->environment()->DebugLog(str);
		}
		if(_claimers.count(val) <1){
			_claimers[val].push_back(claimer);
			if(_ctx->environment()!=NULL){
				std::ostringstream strs;
				strs << "Succeed to ClaimValue since I'm alone  " << val <<endl;
				std::string str = strs.str();
				_ctx->environment()->DebugLog(str);
			}
			return true;
		}
		int index = -1;
		unsigned int cnt=0;
		for(list<ClaimJob<T>*>::iterator it= _claimers.at(val).begin(); it!=_claimers.at(val).end(); ++it)
		{
			if( (*it) == claimer){
				index=cnt;
				break;
			}
			cnt++;
		}
		if (index < 0) // claimer is not found
		{
			index = _claimers.at(val).size();
			_claimers[val].push_back(claimer);
		}
		else
		{
			// we already claimed it
			// this occurs when we were waiting to claim the value but couldn't because of capacity
		}

		// check whether we succeeded claiming
		// if index of claimer on the list is larger than the capacity
		if (index < GetCapacity(val))
		{
			if(_ctx->environment()!=NULL){
				std::ostringstream strs;
				strs << "Succeed to ClaimValue  " << val <<endl;
				std::string str = strs.str();
				_ctx->environment()->DebugLog(str);
			}
			return true;
		}
		if(_ctx->environment()!=NULL){
			std::ostringstream strs;
			strs << "Failed to ClaimValue  " << val <<endl;
			std::string str = strs.str();
			_ctx->environment()->DebugLog(str);
		}
		return false;
	}

	/// <summary>
	/// we assume here that values are released FIFO
	/// </summary>
	/// <param name="val"></param>
	template <typename T> void ClaimReleaseResourceHandler<T>::ReleaseValue(T val)
	{
		if(_ctx->environment()!=NULL){
			std::ostringstream strs;
			strs << "ReleaseValue  " << val;
			std::string str = strs.str();
			_ctx->environment()->DebugLog(str);
		}
		if(_claimers.count(val)<1)
		{
			throw std::invalid_argument("Releasing unclaimed value");
		}
		int number_rem_claimers = _claimers.at(val).size();
		if(_ctx->environment()!=NULL){
			std::ostringstream strs;
			strs << "ReleaseValue  "<< val << " remaining claimers " << number_rem_claimers <<endl;
			std::string str = strs.str();
			_ctx->environment()->DebugLog(str);
		}

		if (number_rem_claimers == 0)
		{
			return;
		}
		_claimers.at(val).pop_front();

		if (_claimers.at(val).size() == 0)
		{
			return;
		}

		ClaimJob<T>* next = GetNextWaitingClaimer(val);
		if(next!=NULL)next->Claim();
	}
	/// <summary>
	/// we assume here that values are released FIFO
	/// </summary>
	/// <param name="val"></param>
	template <typename T> void ClaimReleaseResourceHandler<T>::RemoveClaimer(ClaimJob<T> *job)
	{
		if(job==NULL)return;

		long val = job->key();
		if(_ctx->environment()!=NULL){
			std::ostringstream strs;
			strs << "RemoveClaimer  "<< val << endl;
			std::string str = strs.str();
			_ctx->environment()->DebugLog(str);
		}

		if (IsValueClaimed(val))
		{
			throw std::invalid_argument("Releasing unclaimed value");
		}
		std::size_t nb_claimers = _claimers.at(val).size();
		if (nb_claimers == 0)
		{
			return;
		}
		_claimers.at(val).erase(std::remove(_claimers.at(val).begin(), _claimers.at(val).end(), job), _claimers.at(val).end());

		if (nb_claimers > _claimers.at(val).size() ) // claimer was removed
		{
			if (_claimers.at(val).size() == 0)
			{
				return;
			}

			ClaimJob<T>* next = GetNextWaitingClaimer(val);
			if (next != NULL) next->Claim();
		}
		else
		{
			throw std::invalid_argument("Removing unclaimed claimer");
		}

	}
	template <typename T> ClaimJob<T>* ClaimReleaseResourceHandler<T>::GetNextWaitingClaimer(T val)
	{
		if(_claimers.count(val)< 1) return NULL;
		list<ClaimJob<T> *> l= _claimers.at(val);
		if (l.size() == 0) return NULL;
		std::size_t cap = GetCapacity(val);
		if ( cap== 1) return l.front();
		else
		{
			if (l.size() < cap) return NULL;
			unsigned int cnt=0;
			for(list<ClaimJob<T>*>::iterator it= _claimers.at(val).begin(); it!=_claimers.at(val).end(); ++it){
				if(cnt==cap-1)return (*it);
				cnt++;
			}
		}
	}
	template <typename T> void ClaimReleaseResourceHandler<T>::ResetAll()
	{
		_claimers.clear();
	}

}




